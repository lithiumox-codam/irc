#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

/**
 * @brief The PASS command is used authenticate a user with the server.
 *
 * @param stream The IRStream to write the response to.
 * @param args The arguments of the PASS command.
 * @param user The user that sent the PASS command.
 * @return true If the PASS command was successful.
 */
bool PASS(IRStream &stream, string &args, User *user) {
	if (args.empty()) {
		stream.str("");
		stream.code(ERR_NEEDMOREPARAMS).trail("Not enough parameters").end();

		return false;
	}
	if (user->hasHandshake(U_AUTHENTICATED)) {
		stream.str("");
		stream.code(ERR_ALREADYREGISTRED).param(user->getNickname()).trail("You are already authenticated").end();
		return false;
	}
	if (args == server.getPassword()) {
		user->addHandshake(U_AUTHENTICATED);
	} else {
		return false;
	}

	return true;
}
