#include <algorithm>
#include <vector>

#include "Channel.hpp"
#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

using namespace std;

static string getMessage(vector<string> &tokens) {
	string message;

	if (tokens.size() < 2) {
		for (size_t i = 2; i < tokens.size(); i++) {
			message += ' ' + tokens[i];
		}
	} else {
		message = "No reason provided";
	}

	return message;
}

void KICK(IRStream &stream, string &args, User *user) {
	if (!user->hasHandshake(H_AUTHENTICATED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return;
	}

	vector<string> tokens = split(args, ' ');
	if (tokens.size() < 2) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}
	try {
		Channel *channel = server.getChannel(tokens[0]);
		if (!channel->hasUser(user)) {
			throw NotOnChannelException(tokens[0]);
		}
		Member *target = channel->getMember(server.getUser(tokens[1]));
		if (target == nullptr) {
			throw UserNotOnChannelException(tokens[1]);
		}
		Member *kicker = channel->getMember(user);
		if (!kicker->second.has(M_OPERATOR)) {
			throw UserNotOperatorException();
		}

		stream.prefix(user).command().param(tokens[0]).param(tokens[1]).trail(getMessage(tokens)).end();

		channel->broadcast(stream, kicker->first);
		channel->removeUser(target->first);
	} catch (const IrcException &e) {
		e.e_stream(stream, user);
	}
}
