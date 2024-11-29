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
	if (tokens.size() == 2 && tokens[0].find('o') != string::npos && !server.operatorCheck(user)) {
		stream.prefix().code(ERR_NOPRIVILEGES).param(user->getNickname()).trail("Permission denied").end();
		return;
	}
	try {
		switch (tokens.size()) {
			case 1: {
				auto *member = server.getUser(tokens[0]);
				if (member->getNickname() != user->getNickname()) {
					throw NoOtherUserModeException();
				}
				stream.prefix()
					.code(RPL_UMODEIS)
					.param(user->getNickname())
					.trail("User modes: +" + user->modes.getModesString())
					.end();
			} break;

			case 2: {
				User *target = (tokens[1] != user->getNickname()) ? server.getUser(tokens[1]) : user;
				if (target->getNickname() != user->getNickname() && !server.operatorCheck(user)) {
					throw NoOtherUserModeException();
				}
				auto unsupportedModes = target->modes.applyModeChanges(tokens[0]);
				if (!unsupportedModes.empty()) {
					stream.prefix()
						.code(ERR_UNKNOWNMODE)
						.param(user->getNickname())
						.trail("Unsupported modes: " + unsupportedModes)
						.end();
					return;
				}
				stream.prefix()
					.code(RPL_UMODEIS)
					.param(user->getNickname())
					.trail(target->getNickname() + "'s modes: +" + target->modes.getModesString())
					.end();
			} break;
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
			throw UserNotOperatorException();
		}

		switch (tokens.size()) {
			case 1: {
				stream.prefix()
					.code(RPL_CHANNELMODEIS)
					.param(user->getNickname())
					.param(channel->getName())
					.trail("+" + channel->modes.getModesString())
					.end();
			} break;

			case 2: {
				if (tokens[1].starts_with("+") || tokens[1].starts_with("-")) {
					channel->modes.applyModeChanges(tokens[1]);
					stream.prefix()
						.code(RPL_CHANNELMODEIS)
						.param(user->getNickname())
						.param(channel->getName())
						.trail("+" + channel->modes.getModesString())
						.end();
				} else {
					auto *targetMember = channel->getMember(tokens[1]);
					stream.prefix()
						.code(RPL_UMODEIS)
						.param(user->getNickname())
						.trail(targetMember->first->getNickname() + " User modes: +" +
							   targetMember->second.getModesString())
						.end();
				}
			} break;

			case 3: {
				auto *targetMember = channel->getMember(tokens[2]);
				auto unsupportedModes = targetMember->second.applyModeChanges(tokens[1]);
				if (!unsupportedModes.empty()) {
					stream.prefix()
						.code(ERR_UNKNOWNMODE)
						.param(user->getNickname())
						.trail("Unsupported modes: " + unsupportedModes)
						.end();
					return;
				}
			} break;
		}
	} catch (const IrcException &e) {
		e.e_stream(stream, user);
	}
}

void MODE(IRStream &stream, string &args, User *user) {
	if (args.empty()) {
		throw NotEnoughParametersException();
	}

	vector<string> tokens = split(args, ' ');

	if (tokens.front().starts_with("#")) {
		handleChannelMode(stream, tokens, user);
	} else {
		handleUserMode(stream, tokens, user);
	}
}
