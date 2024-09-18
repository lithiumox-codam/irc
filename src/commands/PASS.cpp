#include "General.hpp"
#include "Server.hpp"

extern Server server;

bool PASS(stringstream &stream, string &args, User &user) {
	if (args.empty()) {
		stream.str("");
		stream << "ERROR :Closing Link: " << server.getHostname() << " (Bad Password)" << END;
		return false;
	}
	if (user.hasHandshake(U_AUTHENTICATED)) {
		stream.str("");
		stream << startRes(ERR_ALREADYREGISTRED) << user.getNickname() << " :You are already registered" << END;
		return false;
	}
	if (args == server.getPassword()) {
		user.addHandshake(U_AUTHENTICATED);
	} else {
		return false;
	}

	return true;
}
