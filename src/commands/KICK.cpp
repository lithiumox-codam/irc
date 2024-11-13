
#include <stdexcept>

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

void KICK(IRStream &stream, string &args, User *user) {
	vector<string> tokens = split(args, ' ');
	if (tokens.size() < 2) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}
	try {
		Channel *channel = server.getChannel(tokens[0]);
		if (!channel->hasUser(user)) {
			// stream.prefix().code(ERR_NOTONCHANNEL).param(user->getNickname()).trail("You're not in that channel").end();
			// return;
			throw NotOnChannelException();
		}
		pair<User *, Modes> *target = channel->getMember(server.getUser(tokens[1]));
		if (target == nullptr) {
			// stream.prefix().code(ERR_USERNOTINCHANNEL).param(user->getNickname()).trail("User not in channel").end();
			// return;
			throw UserNotOnChannelException();
		}
		pair<User *, Modes> *kicker = channel->getMember(user);
		if (!kicker->second.hasModes(M_OPERATOR)) {
			// stream.prefix()
			// 	.code(ERR_CHANOPRIVSNEEDED)
			// 	.param(user->getNickname())
			// 	.trail("You're not a channel operator")
			// 	.end();
			// return;
			throw UserNotOperatorException();
		}

		stream.prefix(user).command().param(channel->getName()).param(target->first->getNickname());
		if (tokens.size() > 2) {
			stream.param(join(tokens, 2));
		} else {
			stream.param(kicker->first->getNickname());
		}
		stream.end();
		channel->broadcast2(stream, kicker->first);
		channel->removeUser(target->first);
	} catch (const IrcException &e) {
		e.e_stream(stream, user);
	}
}
