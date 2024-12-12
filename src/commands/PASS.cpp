#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

/**
 * @brief The PASS command is used authenticate a user with the server.
 *
 * @param stream The IRStream to write the response to.
 * @param args The arguments of the PASS command.
 * @param user The user that sent the PASS command.
 * @return true If the PASS command was successful.
 */
void PASS(IRStream &stream, string &args, User *user) {	 // if pass fails, disconnect user
	if (args.empty()) {
		stream.str("");
		stream.code(ERR_NEEDMOREPARAMS).trail("Not enough parameters").end();

		return;
	}
	if (user->hasHandshake(H_PASS)) {
		stream.str("");
		stream.code(ERR_ALREADYREGISTRED).param(user->getNickname()).trail("You are already authenticated").end();
		return;
	}

	if (args == server.getPassword()) {
		user->addHandshake(H_PASS);
	} else {
		stream.prefix().code(ERR_PASSWDMISMATCH).param(user->getNickname()).trail("Password incorrect").end();
		return;
	}

	if (user->hasHandshake(H_AUTHENTICATED) && !user->hasHandshake(H_WELCOME)) {
		string empty;
		MOTD(stream, empty, user);
		user->addHandshake(H_WELCOME);
	}
}
