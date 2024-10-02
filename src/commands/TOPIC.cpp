#include <iostream>
#include <stdexcept>

#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

static void correctTrail(vector<string> &tokens) {
	string trail;
	size_t i = 0;
	while (i < tokens.size()) {
		if (tokens[i][0] == ':') {
			for (size_t j = i; j < tokens.size(); j++) {
				trail += tokens[j];
				if (j != tokens.size() - 1) {
					trail += " ";
				}
			}
			break;
		}
		i++;
	}
	tokens.erase(tokens.begin() + i, tokens.end());
	trail.erase(trail.begin());
	tokens.push_back(trail);
}

bool TOPIC(IRStream &stream, string &args, User *user) {
	if (!user->hasHandshake(U_COMPLETED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return false;
	}
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return false;
	}
	auto tokens = split(args, ' ');
	if (tokens.size() == 1) {
		if (tokens[0][0] != '#') {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
			return false;
		}
		try {
			Channel *channel = server.getChannel(tokens[0]);
			if (!channel->hasUser(user)) {
				stream.prefix()
					.code(ERR_NOTONCHANNEL)
					.param(user->getNickname())
					.trail("You're not in that channel")
					.end();
				return false;
			}
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
			cout << "topic: " << channel->getTopic() << "\n";
			return true;
		} catch (runtime_error &e) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
			return false;
		}
	}
	if (tokens[1][0] == ':' && tokens[1][1] == ':') {
		Channel *channel = server.getChannel(tokens[0]);
		if (!channel->hasUser(user)) {
			stream.prefix().code(ERR_NOTONCHANNEL).param(user->getNickname()).trail("You're not in that channel").end();
			return false;
		}
		channel->setTopic("");
		stream.prefix()
			.code(RPL_NOTOPIC)
			.param(user->getNickname())
			.param(channel->getName())
			.trail("Topic removed")
			.end();
	}
	correctTrail(tokens);
	if (tokens.size() == 2) {
		if (tokens[0][0] != '#') {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
			return false;
		}
		try {
			Channel *channel = server.getChannel(tokens[0]);
			if (!channel->hasUser(user)) {
				stream.prefix()
					.code(ERR_NOTONCHANNEL)
					.param(user->getNickname())
					.trail("You're not in that channel")
					.end();
				return false;
			}
			channel->setTopic(tokens[1]);
			stream.prefix()
				.code(RPL_TOPIC)
				.param(user->getNickname())
				.param(channel->getName())
				.trail(channel->getTopic())
				.end();
			return true;
		} catch (runtime_error &e) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
			return false;
		}
	}
	stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
	return false;
}
