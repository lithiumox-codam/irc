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
	if (!user->hasHandshake(USER_AUTHENTICATED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return;
	}

	pair<string, string> tokens = splitPair(args, ' ');
	string inviteeName = tokens.first;
	string channelName = tokens.second;

	try {
		if (channelName.empty() || inviteeName.empty()) {
			throw NotEnoughParametersException();
		}

		Channel *channel = server.getChannel(channelName);
		pair<User *, Modes> *inviter = channel->getMember(user);

		if (inviter == nullptr) {
			throw NotOnChannelException(user->getNickname());
		}

		if (channel->modes.hasModes(M_INVITE_ONLY) && !inviter->second.hasModes(M_OPERATOR)) {
			throw UserNotOperatorException();
		}

		User *invitee = server.getUser(inviteeName);

		if (channel->getMember(invitee) != nullptr) {
			throw UserAlreadyOnChannelException();
		}

		channel->addInvited(invitee);

		stream.prefix().code(RPL_INVITING).param(user->getNickname()).param(inviteeName).param(channelName).end();
		IRStream invStream;
		invStream.prefix(user).param("INVITE").param(inviteeName).param(channelName).end().sendPacket(invitee);

	} catch (const IrcException &e) {
		e.e_stream(stream, user);
	}
}
