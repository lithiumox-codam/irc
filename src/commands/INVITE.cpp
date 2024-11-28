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

void INVITE(IRStream &stream, string &args, User *user) {
	pair<string, string> tokens = splitPair(args, ' ');

	if (tokens.first.empty() || tokens.second.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}
	try {
		Channel *channel = server.getChannel(tokens.first);
		pair<User *, Modes> *inviter = channel->getMember(user);
		if (inviter == nullptr) {
			throw NotOnChannelException();
		}
		if (channel->modes.hasModes(M_INVITE_ONLY) && !inviter->second.hasModes(M_OPERATOR)) {
			throw UserNotOperatorException();
		}
		User *invitee = server.getUser(tokens.second);
		if (channel->getMember(invitee) != nullptr) {
			throw UserAlreadyOnChannelException();
		}
		channel->addInvited(invitee);
		stream.prefix().code(RPL_INVITING).param(user->getNickname()).param(tokens.second).param(tokens.first).end();
		IRStream invStream;
		invStream.prefix(user).param("INVITE").param(tokens.second).param(tokens.first).end().sendPacket(invitee);
	} catch (const IrcException &e) {
		e.e_stream(stream, user);
	}
}
