#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

void INFO(IRStream &stream, string &args, User *user) {
	(void)args;
	string message;

	if (!user->hasHandshake(H_AUTHENTICATED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return;
	}

	stream.prefix().code(RPL_INFO).param(user->getNickname()).trail(server.getHostname() + " - Server").end();
	stream.prefix().code(RPL_INFO).param(user->getNickname()).trail("Version 1.0").end();
	stream.prefix().code(RPL_INFO).param(user->getNickname()).trail("Created by: Ole, Max & Mees").end();
	stream.prefix()
		.code(RPL_INFO)
		.param(user->getNickname())
		.trail("You are known as " + user->getNickname() + " have fun chatting 😃")
		.end();
	stream.prefix()
		.code(RPL_INFO)
		.param(user->getNickname())
		.trail("There are currently " + server.getUserCount() + " clients connected")
		.end();
	stream.prefix().code(RPL_ENDOFINFO).param(user->getNickname()).trail("End of /INFO list").end();
}
