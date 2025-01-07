#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

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
	cerr << "diff: " << diff << '\n';
	return diff;
}

static string diffModes(const string &modes){
	vector<string> modeGroups;
	stringstream currentGroup;

	for (char chr : modes) {
		if (chr == '+' || chr == '-') {
			if (!currentGroup.str().empty()) {
				modeGroups.push_back(currentGroup.str());
			}
			currentGroup.str(string());
			currentGroup << chr;
		} else {
			currentGroup << chr;
		}
	}
	if (!currentGroup.str().empty()) {
		modeGroups.push_back(currentGroup.str());
	}

	for (auto &group : modeGroups) {
		char sign = group[0];
		unordered_set<char> seenModes;
		string cleanedGroup(1, sign);

		for (size_t i = 1; i < group.size(); ++i) {
			char chr = group[i];
			if (seenModes.find(chr) == seenModes.end()) {
				cleanedGroup += chr;
				seenModes.insert(chr);
			}
		}
		group = cleanedGroup;
	}

	string result;
	for (const auto &group : modeGroups) {
		result += group;
	}
	return result;
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

static void channelOperatorHelper(const string &mode, User *user, Channel *channel, string &targetnick) {
	auto *target = channel->getMember(targetnick);

	if (!channel->hasOperator(user)) {
		throw UserNotOperatorException();
	}
	if (mode[0] == '+') {
		channel->addOperator(target->first);
	} else {
		channel->removeOperator(target->first);
	}
	broadcastModeChange(channel, user, mode, targetnick);
}

/**
* @brief Handles the MODE command for users. This function will handle the following:
* - MODE user           : Returns the modes of the user.
* - MODE user (+|-)mode : (Un)sets a mode from the user. Only operators can change other users' modes.
*
* @param stream The stream to output to.
* @param tokens The tokens from the command.
* @param user The user that sent the command.
*/
static void handleUserMode(IRStream &stream, vector<string> &tokens, User *user) {
	switch (tokens.size()) {
		case 1: {
			if (tokens[0] != user->getNickname() && !server.operatorCheck(user)) {
				stream.prefix().code(ERR_NOPRIVILEGES).param(user->getNickname()).trail("Permission denied").end();
				return;
			}
			auto *target = (tokens[0] != user->getNickname()) ? server.getUser(tokens[0]) : user;
			sendModeChange(stream, user, target->modes.getString());
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
			auto unsupportedModes = target->modes.applyChanges(tokens[1]);
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
}

/**
* @brief Handles the MODE command for channels. This function will handle the following:
* - MODE #channel                : Returns the modes of the channel.
* - MODE #channel (+|-)mode      : (Un)sets a mode from the channel.
* - MODE #channel user           : Returns the modes of the user in the channel.
* - MODE #channel (+|-)mode user : (Un)sets the modes of the user in the channel. Only for operators.
*
* @param stream The stream to output to.
* @param tokens The tokens from the command.
* @param user The user that sent the command.
*/
static void handleChannelModes(IRStream &stream, vector<string> &tokens, User *user) {
	char sign = '\0';
	Channel *channel = server.getChannel(tokens[0]);
	if (tokens.size() == 1) {
		stream.prefix().code(RPL_CHANNELMODEIS).param(user->getNickname()).param(tokens[0]).param(channel->modes.getString()).end();
		return;
	}
	if (!channel->hasOperator(user)) {
		throw UserNotOperatorException();
	}
	string modes = diffModes(tokens[1]);
	auto tokenIt = tokens.begin() + 2;

	for (size_t i = 0; i < modes.length(); ++i) {
		char chr = modes[i];

		if (chr == '+' || chr == '-') {
			sign = chr;
			continue;
		}
		try {
			switch (chr) {
				case 'o':
					if (tokenIt != tokens.end()) {
						channelOperatorHelper({sign, chr}, user, channel, *tokenIt);
					} else {
						throw NotEnoughParametersException();
					}
					break;
				case 'l':
					if (sign == '+') {
						tokenIt != tokens.end() ? channel->setLimit(stoi(*tokenIt)) : throw NotEnoughParametersException();
						broadcastModeChange(channel, user, {sign, chr}, *tokenIt);
						++tokenIt;
					} else {
						channel->removeLimit();
						broadcastModeChange(channel, user, {sign, chr}, "");
					}
					break;
				case 'k':
					if (sign == '+') {
						tokenIt != tokens.end() ? channel->setPassword(*tokenIt) : throw NotEnoughParametersException();
						broadcastModeChange(channel, user, {sign, chr}, *tokenIt);
						++tokenIt;
					} else {
						channel->removePassword();
						broadcastModeChange(channel, user, {sign, chr}, channel->getName());
					}
					break;
				case 'i':
					if (sign == '+') {
						channel->modes.add(M_INVITE_ONLY);
						broadcastModeChange(channel, user, {sign, chr}, "");
					} else {
						channel->modes.remove(M_INVITE_ONLY);
						broadcastModeChange(channel, user, {sign, chr}, "");
					}
					break;
				case 't':
					if (sign == '+') {
						channel->modes.add(M_TOPIC_LOCK);
						broadcastModeChange(channel, user, {sign, chr}, "");
					} else {
						channel->modes.remove(M_TOPIC_LOCK);
						broadcastModeChange(channel, user, {sign, chr}, "");
					}
					break;
				default:
					auto unsupportedModes = channel->modes.applyChanges({sign, chr});
					if (!unsupportedModes.empty()) {
						sendUnknownMode(stream, user, unsupportedModes);
					}
					break;
			}
		} catch (const IrcException &e) {
			e.e_stream(stream, user);
		} catch (const exception &e) {
			NotEnoughParametersException().e_stream(stream, user);
		}
	}
}

void MODE(IRStream &stream, string &args, User *user) {
	if (!user->hasHandshake(H_AUTHENTICATED)) {
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

	try {
		if (tokens.front().starts_with("#")) {
			handleChannelModes(stream, tokens, user);
		} else {
			handleUserMode(stream, tokens, user);
		}
	} catch (const IrcException &e) {
		e.e_stream(stream, user);
	}
}

// static void oHandler(IRStream &stream, vector<string> &args, User *user, char sign) {
// 	if (args.size() < 3) {
// 		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
// 		return;
// 	}

// 	auto *channel = server.getChannel(args[0]);
// 	auto *member = channel->getMember(user->getNickname());

// 	if (!member->second.has(M_OPERATOR)) {
// 		stream.prefix()
// 			.code(ERR_CHANOPRIVSNEEDED)
// 			.param(user->getNickname())
// 			.param(channel->getName() + ":")
// 			.trail("You're not channel operator")
// 			.end();
// 		return;
// 	}

// 	auto *target = channel->getMember(args[2]);
// 	if (target == nullptr) {
// 		stream.prefix()
// 			.code(ERR_USERNOTINCHANNEL)
// 			.param(user->getNickname())
// 			.param(args[2])
// 			.param(channel->getName())
// 			.trail("They aren't on that channel")
// 			.end();
// 		return;
// 	}

// 	if (sign == '+') {
// 		target->second.add(M_OPERATOR);
// 	} else {
// 		target->second.remove(M_OPERATOR);
// 	}

// 	broadcastModeChange(channel, user, string(1, sign) + "o", args[2]);
// }

