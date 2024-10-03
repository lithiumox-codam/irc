#include <iostream>

#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

using namespace std;

bool PRIVMSG(IRStream &stream, string &args, User *user) {
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return false;
	}
	pair<string, string> token = splitPair(args, ' ');
	if (token.first.empty() || token.second.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return false;
	}
	if (token.first[0] != '#') {
		if (token.first == user->getNickname()) {
			stream.prefix()
				.code(ERR_NOSUCHNICK)
				.param(user->getNickname())
				.trail("Error: Talking to yourself is not allowed!")
				.end();
			return false;
		}
		try {
			IRStream targetStream;
			User *target = server.getUser(token.first);
			targetStream.prefix(user).param("PRIVMSG").param(token.first).trail(token.second).end().sendPacket(target);

		} catch (const runtime_error &e) {
			stream.prefix().code(ERR_NOSUCHNICK).param(user->getNickname()).trail("No such nick/channel").end();
			return false;
		}
	} else {
		try {
			Channel *channel = server.getChannel(token.first);
			if (!channel->hasUser(user)) {
				stream.prefix()
					.code(ERR_NOTONCHANNEL)
					.param(user->getNickname())
					.param(token.first)
					.trail("You're not on that channel")
					.end();
				return false;
			}
			cerr << "sending message: " << token.second << " to channel: " << token.first << "\n";
			channel->broadcast(user, token.second);
		} catch (const runtime_error &e) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
			return false;
		}
	}
	return true;
}
