#include <iostream>

#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

bool INFO(IRStream &stream, string &args, User &user) {
	(void)args;
	const std::string &nickname = user.getNickname();

	stream.prefix().code(RPL_INFO).param(nickname).trail(server.getHostname() + " - Server").end();
	stream.prefix().code(RPL_INFO).param(nickname).trail("Version 1.0").end();
	stream.prefix().code(RPL_INFO).param(nickname).trail("Created by: Ole, Max & Mees").end();
	stream.prefix()
		.code(RPL_INFO)
		.param(nickname)
		.trail("You are known as " + user.getRealname() + " have fun chatting 😃")
		.end();
	stream.prefix().code(RPL_ENDOFINFO).param(nickname).trail("End of /INFO list").end();

	return true;
}
