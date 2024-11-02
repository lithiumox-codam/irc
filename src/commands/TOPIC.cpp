#include <iostream>
#include <stdexcept>
#include <string>

#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"

extern Server server;

void TOPIC(IRStream &stream, string &args, User *user) {
	if (!user->hasHandshake(USER_REGISTERED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return ;
	}
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return ;
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
			return ;
		}
		if (tokens.second.empty()) {
			if (channel->getTopic().empty()) {
				stream.prefix()
					.code(RPL_NOTOPIC)
					.param(user->getNickname())
					.param(channel->getName())
					.trail("No topic is set")
					.end();
				return ;
			}
			stream.prefix()
				.code(RPL_TOPIC)
				.param(user->getNickname())
				.param(channel->getName())
				.trail(channel->getTopic())
				.end();
				return ;
		}
		if (tokens.second == "::") {
			channel->setTopic("");
			stream.prefix()
				.param(user->getNickname())
				.param(channel->getName())
				.trail(":")
				.end();
				return ;
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
		return ;
	} catch (runtime_error &e) {
		stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
	}
}
