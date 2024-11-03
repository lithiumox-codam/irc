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
			stream.prefix().code(ERR_NOTONCHANNEL).param(user->getNickname()).trail("You're not in that channel").end();
			return;
		}
		if (channel->modes.hasModes(M_INVITE_ONLY) && !inviter->second.hasModes(M_OPERATOR)) {
			stream.prefix()
				.code(ERR_CHANOPRIVSNEEDED)
				.param(user->getNickname())
				.trail("You're not a channel operator")
				.end();
			return;
		}
		User *invitee = server.getUser(tokens.second);
		if (channel->getMember(invitee) != nullptr) {
			stream.prefix()
				.code(ERR_USERONCHANNEL)
				.param(user->getNickname())
				.trail("User is already on channel")
				.end();
			return;
		}
		channel->addInvited(invitee);
		stream.prefix().code(RPL_INVITING).param(user->getNickname()).param(tokens.second).param(tokens.first).end();
		IRStream invStream;
		invStream.prefix(user).param("INVITE").param(tokens.second).param(tokens.first).end().sendPacket(invitee);
	} catch (runtime_error &e) {
		if (e.what() == string("User not found")) {
			stream.prefix().code(ERR_NOSUCHNICK).param(user->getNickname()).trail("No such nickname").end();
			return;
		}
		stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
	}
}
