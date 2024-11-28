#include <iostream>
#include <stdexcept>
#include <string>

#include "Channel.hpp"
#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

string applyModeChanges(Modes &modes, const std::string &modeChanges) {
	static constexpr std::pair<char, unsigned int> modePairs[] = {
		{'o', M_OPERATOR}, {'v', M_VOICE},		{'i', M_INVISIBLE}, {'m', M_MODERATED},
		{'k', M_PASSWORD}, {'t', M_TOPIC_LOCK}, {'l', M_LIMIT}};

	string unsupportedModes;
	bool addMode = false;

	for (char modeChar : modeChanges) {
		if (modeChar == '+' || modeChar == '-') {
			addMode = (modeChar == '+');
			continue;
		}

		const auto *iter = ranges::find_if(modePairs, [modeChar](const auto &pair) { return pair.first == modeChar; });

		if (iter != std::end(modePairs)) {
			if (addMode) {
				modes.addModes(iter->second);
			} else {
				modes.removeModes(iter->second);
			}
		} else {
			unsupportedModes.push_back(modeChar);
		}
	}

	return unsupportedModes;
}

void handleUserMode(IRStream &stream, vector<string> &tokens, User *user) {
	if (tokens.size() == 2 && tokens[1] != user->getNickname()) {
		stream.prefix()
			.code(ERR_USERSDONTMATCH)
			.param(user->getNickname())
			.trail("Cannot change modes for other users")
			.end();
		return;
	}

	if (tokens.size() == 1) {
		try {
			auto *member = server.getUser(tokens[0]);
			if (member->getNickname() != user->getNickname()) {
				throw NoOtherUserModeException();
			}
			stream.prefix()
				.code(RPL_UMODEIS)
				.param(user->getNickname())
				.trail("User modes: +" + user->modes.getModesString())
				.end();
			return;
		} catch (const IrcException &e) {
			e.e_stream(stream, user);
			return;
		}
	}
	if (tokens.size() == 2) {
		auto unsupportedModes = applyModeChanges(user->modes, tokens[1]);
		if (!unsupportedModes.empty()) {
			stream.prefix().code(ERR_UNKNOWNMODE).trail("Unsupported modes: " + unsupportedModes).end();
		}
		stream.prefix()
			.code(RPL_UMODEIS)
			.param(user->getNickname())
			.trail("User modes: +" + user->modes.getModesString())
			.end();
		return;
	}
	stream.prefix().code(ERR_UNKNOWNMODE).trail("Unknown mode").end();
}

void handleChannelMode(IRStream &stream, vector<string> &tokens, User *user) {
	try {
		auto *channel = server.getChannel(tokens.front());
		auto member = channel->getMember(user->getNickname());

		if (!member.second.hasModes(M_OPERATOR)) {
			throw UserNotOperatorException();
		}

		if (tokens.size() == 1) {
			stream.prefix()
				.code(RPL_CHANNELMODEIS)
				.param(user->getNickname())
				.param(channel->getName())
				.trail("+" + channel->modes.getModesString())
				.end();
			return;
		}
		if (tokens.size() == 2) {
			if (tokens[1].starts_with("+") || tokens[1].starts_with("-")) {
				applyModeChanges(channel->modes, tokens[1]);
				stream.prefix()
					.code(RPL_CHANNELMODEIS)
					.param(user->getNickname())
					.param(channel->getName())
					.trail("+" + channel->modes.getModesString())
					.end();
				return;
			}
			try {
				auto &member = channel->getMember(tokens[1]);
				stream.prefix()
					.code(RPL_UMODEIS)
					.param(user->getNickname())
					.trail(member.first->getNickname() + " User modes: +" + member.second.getModesString())
					.end();
			} catch (const IrcException &e) {
				e.e_stream(stream, user);
				return;
			}
		}
		if (tokens.size() == 3) {
			try {
				auto &member = channel->getMember(tokens[2]);
				auto unsupportedModes = applyModeChanges(member.second, tokens[1]);
				if (!unsupportedModes.empty()) {
					stream.prefix().code(ERR_UNKNOWNMODE).trail("Unsupported modes: " + unsupportedModes).end();
					return;
				}
			} catch (const IrcException &e) {
				e.e_stream(stream, user);
				return;
			}
		}
	} catch (const IrcException &e) {
		e.e_stream(stream, user);
		return;
	}
	stream.prefix().code(ERR_UNKNOWNMODE).trail("Unknown mode").end();
}

void MODE(IRStream &stream, string &args, User *user) {
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).trail("MODE :Not enough parameters").end();
		return;
	}

	vector<string> tokens = split(args, ' ');

	if (tokens.front().starts_with("#")) {
		handleChannelMode(stream, tokens, user);
	} else {
		handleUserMode(stream, tokens, user);
	}
}
