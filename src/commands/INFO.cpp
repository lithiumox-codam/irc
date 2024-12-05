#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

void INFO(IRStream &stream, string &args, User *user) {
	(void)args;
	string message;

	stream.prefix().code(RPL_INFO).param(user->getNickname()).trail(server.getHostname() + " - Server").end();
	stream.prefix().code(RPL_INFO).param(user->getNickname()).trail("Version 1.0").end();
	stream.prefix().code(RPL_INFO).param(user->getNickname()).trail("Created by: Ole, Max & Mees").end();
	
	message = (user->hasHandshake(H_REGISTERED))
		? "You are known as " + user->getNickname() + " have fun chatting 😃"
		: "You are not yet registered";
	stream.prefix().code(RPL_INFO).param(user->getNickname()).trail(message).end();

	message = (server.getUserCount() == "1")
		? "There is currently 1 client connected"
		: "There are currently " + server.getUserCount() +" clients connected";
	stream.prefix().code(RPL_INFO).param(user->getNickname()).trail(message).end();

	stream.prefix().code(RPL_ENDOFINFO).param(user->getNickname()).trail("End of /INFO list").end();
}
