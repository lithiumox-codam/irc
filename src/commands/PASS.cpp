#include "Codes.hpp"
#include "General.hpp"
#include "Server.hpp"

extern Server server;

/**
 * @brief The PASS command is used authenticate a user with the server.
 *
 * @param stream The stringstream to write the response to.
 * @param args The arguments of the PASS command.
 * @param user The user that sent the PASS command.
 * @return true If the PASS command was successful.
 */
bool PASS(stringstream &stream, string &args, User &user) {
	if (args.empty()) {
		stream.str("");
		stream << startRes(ERR_NEEDMOREPARAMS) << user.getNickname() << " PASS :Not enough parameters" << END;
		return false;
	}
	if (user.hasHandshake(U_AUTHENTICATED)) {
		stream.str("");
		stream << startRes(ERR_ALREADYREGISTRED) << user.getNickname() << " :You are already authenticated" << END;
		return false;
	}
	if (args == server.getPassword()) {
		user.addHandshake(U_AUTHENTICATED);
	} else {
		return false;
	}

	return true;
}
