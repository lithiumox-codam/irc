#include "User.hpp"

#include <iostream>

#include "Codes.hpp"
#include "General.hpp"
#include "Server.hpp"

extern Server server;

/**
 * @brief Makes it so that the realname is not split by spaces.
 *
 * @param args The split arguments of the USER command.
 * @return The arguments of the USER command with the realname as one string.
 */
static void correct_realname(vector<string> &args) {
	string realname;
	for (size_t i = 3; i < args.size(); i++) {
		realname += args[i];
		if (i + 1 < args.size()) {
			realname += " ";
		}
	}
	realname.erase(0, realname.find_first_not_of(':'));
	args.erase(args.begin() + 3, args.end());
	args.push_back(realname);
}

/**
 * @brief The USER command is used at the beginning of connection to specify the username, hostname, servername and
 * realname of a new user.
 */
bool USER(stringstream &stream, string &args, User &user) {
	if (user.hasHandshake(U_USER)) {
		stream << startRes(ERR_ALREADYREGISTRED) << user.getNickname() << " :You may not reregister" << END;
		return false;
	}
	vector<string> tokens = split(args, ' ');
	correct_realname(tokens);
	if (tokens.size() < 4) {
		cerr << "Error: USER packet has less than 4 arguments" << "\n";
		return false;
	}

	user.setUsername(tokens[0]);
	user.setRealname(tokens[3]);
	user.setHostname(tokens[2]);
	user.addHandshake(U_USER);

	return true;
}
