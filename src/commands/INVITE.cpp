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
	if (!user->hasHandshake(H_AUTHENTICATED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return;
	}
	
	auto [InviteeName, ChannelName] = splitPair(args, ' ');

	if (ChannelName.empty() || InviteeName.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}
	try {
		Channel *channel = server.getChannel(ChannelName);
		auto *inviter = channel->getMember(user);
		if (inviter == nullptr) {
			throw NotOnChannelException(user->getNickname());
		}

		if (channel->modes.has(M_INVITE_ONLY) && !inviter->second.has(M_OPERATOR)) {
			throw UserNotOperatorException();
		}
		User *invitee = server.getUser(InviteeName);
		if (channel->getMember(invitee) != nullptr) {
			throw UserAlreadyOnChannelException();
		}
		channel->addInvited(invitee);
		stream.prefix().code(RPL_INVITING).param(user->getNickname()).param(InviteeName).param(ChannelName).end();
		IRStream invStream;
		invStream.prefix(user).param("INVITE").param(InviteeName).trail(ChannelName).end().sendPacket(invitee);
	} catch (const IrcException &e) {
		e.e_stream(stream, user);
	}
}
