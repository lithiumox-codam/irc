#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Channel.hpp"
#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

static string diffModes(const string &modes, const string &unsupportedModes) {
	string diff;
	unordered_map<char, char> modeMap;
	vector<char> modeOrder;
	char currentOp = 0;

	for (size_t i = 0; i < modes.length(); ++i) {
		char chr = modes[i];
		if (chr == '+' || chr == '-') {
			currentOp = chr;
			continue;
		}
		if (unsupportedModes.find(chr) != string::npos) {
			continue;
		}
		modeMap[chr] = currentOp;
		if (ranges::find(modeOrder.begin(), modeOrder.end(), chr) == modeOrder.end()) {
			modeOrder.push_back(chr);
		}
	}

	currentOp = 0;
	for (char chr : modeOrder) {
		if (modeMap[chr] != currentOp) {
			diff += modeMap[chr];
			currentOp = modeMap[chr];
		}
		diff += chr;
	}

	return diff;
}

static void serverOperatorHelper(const string &modes, User *user) {
	bool addMode = false;

	if (modes.find('o') == string::npos) {
		return;
	}

	for (const auto &mode : modes) {
		if (mode == '+' || mode == '-') {
			addMode = (mode == '+');
			continue;
		}
		if (mode != 'o') {
			continue;
		}
		if (addMode) {
			server.addOperator(user->getNickname());
		} else {
			server.removeOperator(user->getNickname());
		}
	}
}

static void broadcastModeChange(Channel *channel, User *user, const string &modes, const string &target) {
	IRStream stream;
	stream.prefix(user).param("MODE").param(channel->getName()).param(modes).param(target).end();
	channel->broadcast(stream);
}

static void sendModeChange(IRStream &stream, User *user, const string &modes) {
	stream.prefix(user).code(RPL_UMODEIS).param(user->getNickname()).param(modes).end();
}

static void sendUnknownMode(IRStream &stream, User *user, const string &unsupportedModes) {
	stream.prefix()
		.code(ERR_UNKNOWNMODE)
		.param(user->getNickname())
		.param(unsupportedModes)
		.trail("is not a valid mode in this scope!")
		.end();
}

/**
 * @brief Handles the MODE command for users. This function will handle the following:
 * - MODE user			 : Returns the modes of the user.
 * - MODE user (+|-)mode : (Un)sets a mode from the user. Only operators can change other users' modes.
 *
 * @param stream The stream to output to.
 * @param tokens The tokens from the command.
 * @param user The user that sent the command.
 */
static void handleUserMode(IRStream &stream, vector<string> &tokens, User *user) {
	try {
		switch (tokens.size()) {
			case 1: {
				if (tokens[0] != user->getNickname() && !server.operatorCheck(user)) {
					stream.prefix().code(ERR_NOPRIVILEGES).param(user->getNickname()).trail("Permission denied").end();
					return;
				}
				auto *target = (tokens[0] != user->getNickname()) ? server.getUser(tokens[0]) : user;
				sendModeChange(stream, user, target->modes.getModesString());
			} break;

			case 2: {
				User *target = (tokens[0] != user->getNickname()) ? server.getUser(tokens[0]) : user;
				if (target->getNickname() != user->getNickname() && !server.operatorCheck(user)) {
					throw NoOtherUserModeException();
				}
				if (tokens[1].find('o') != string::npos && !server.operatorCheck(user)) {
					stream.prefix().code(ERR_NOPRIVILEGES).param(user->getNickname()).trail("Permission denied").end();
					return;
				}
				auto unsupportedModes = target->modes.applyModeChanges(tokens[1]);
				serverOperatorHelper(tokens[1], target);
				if (!unsupportedModes.empty()) {
					sendUnknownMode(stream, user, unsupportedModes);
				}
				sendModeChange(stream, user, diffModes(tokens[1], unsupportedModes));
			} break;

			default: {
				throw NotEnoughParametersException();
			}
		}
	} catch (const IrcException &e) {
		e.e_stream(stream, user);
	}
}

/**
 * @brief Handles the MODE command for channels. This function will handle the following:
 * - MODE #channel				  : Returns the modes of the channel.
 * - MODE #channel (+|-)mode	  : (Un)sets a mode from the channel.
 * - MODE #channel user 		  : Returns the modes of the user in the channel.
 * - MODE #channel (+|-)mode user : (Un)sets the modes of the user in the channel. Only for operators.
 *
 * @param stream The stream to output to.
 * @param tokens The tokens from the command.
 * @param user The user that sent the command.
 */
static void handleChannelMode(IRStream &stream, vector<string> &tokens, User *user) {
	try {
		auto *channel = server.getChannel(tokens[0]);
		auto *member = channel->getMember(user->getNickname());

		if (!member->second.hasModes(M_OPERATOR)) {
			stream.prefix()
				.code(ERR_CHANOPRIVSNEEDED)
				.param(user->getNickname())
				.param(channel->getName() + ":")
				.trail("You're not channel operator")
				.end();
			return;
		}

		switch (tokens.size()) {
			case 1: {
				stream.prefix()
					.code(RPL_CHANNELMODEIS)
					.param(user->getNickname())
					.param(channel->getName())
					.param(channel->modes.getModesString())
					.end();
			} break;

			case 2: {
				if (tokens[1].starts_with("+") || tokens[1].starts_with("-")) {
					channel->modes.applyModeChanges(tokens[1]);
					broadcastModeChange(channel, user, tokens[1], "");
				} else {
					auto *targetMember = channel->getMember(tokens[1]);
					sendModeChange(stream, user, targetMember->second.getModesString());
				}
			} break;

			case 3: {
				// Check if token[2] is a number for limit mode
				if (all_of(tokens[2].begin(), tokens[2].end(), ::isdigit)) {
					// Only look for limit mode specifically
					size_t limitPos = tokens[1].find('l');
					if (limitPos != string::npos && tokens[1].rfind('+', limitPos) != string::npos) {
						channel->setLimit(stoi(tokens[2]));
					}
					// Apply all mode changes including other modes
					channel->modes.applyModeChanges(tokens[1]);
					broadcastModeChange(channel, user, tokens[1], tokens[2]);
					break;
				}

				auto *targetMember = channel->getMember(tokens[2]);
				auto unsupportedModes = targetMember->second.applyModeChanges(tokens[1]);
				if (!unsupportedModes.empty()) {
					sendUnknownMode(stream, user, unsupportedModes);
				} else if (!diffModes(tokens[1], unsupportedModes).empty()) {
					broadcastModeChange(channel, user, diffModes(tokens[1], unsupportedModes), tokens[2]);
				}
			} break;

			default: {
				throw NotEnoughParametersException();
			}
		}
	} catch (const IrcException &e) {
		e.e_stream(stream, user);
	}
}

void MODE(IRStream &stream, string &args, User *user) {
	if (!user->hasHandshake(USER_AUTHENTICATED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return;
	}
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}

	vector<string> tokens = split(args, ' ');

	if (tokens.size() > 2 && tokens[1].find('+') == string::npos && tokens[1].find('-') == string::npos) {
		stream.prefix()
			.code(ERR_NEEDMOREPARAMS)
			.param(user->getNickname())
			.trail("A mode string should contain + or -")
			.end();
		return;
	}

	if (tokens.front().starts_with("#")) {
		handleChannelMode(stream, tokens, user);
	} else {
		handleUserMode(stream, tokens, user);
	}
}
