#include <iostream>
#include <stdexcept>
#include <string>

#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"

extern Server server;

bool TOPIC(IRStream &stream, string &args, User *user) {
	if (!user->hasHandshake(U_COMPLETED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return false;
	}
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return false;
	}
	pair<string, string> tokens = splitPair(args, ' ');
	cerr << "parser gives: " << tokens.first << " and [" << tokens.second << "]\n";
	try {
		Channel *channel = server.getChannel(tokens.first);
		if (!channel->hasUser(user)) {
			stream.prefix()
				.code(ERR_NOTONCHANNEL)
				.param(user->getNickname())
				.trail("You're not in that channel")
				.end();
			return false;
		}
		if (tokens.second.empty()) {
			if (channel->getTopic().empty()) {
				stream.prefix()
					.code(RPL_NOTOPIC)
					.param(user->getNickname())
					.param(channel->getName())
					.trail("No topic is set")
					.end();
				return true;
			}
			stream.prefix()
				.code(RPL_TOPIC)
				.param(user->getNickname())
				.param(channel->getName())
				.trail(channel->getTopic())
				.end();
				return true;
		}
		if (tokens.second == "::") {
			channel->setTopic("");
			stream.prefix()
				.param(user->getNickname())
				.param(channel->getName())
				.trail(":")
				.end();
				return true;
		}
		channel->setTopic(tokens.second);
		stream.prefix()
			.code(RPL_TOPIC)
			.param(user->getNickname())
			.param(channel->getName())
			.trail(channel->getTopic())
			.end();
		stream.prefix()
			.code(RPL_TOPICTIME)
			.param(user->getNickname())
			.param(channel->getName())
			.param(user->getNickname())
			.trail(to_string(time(nullptr)))
			.end();
		return true;
	} catch (runtime_error &e) {
		stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
		return false;
	}
	return true;
}
