#include "User.hpp"

#include <iostream>

#include "General.hpp"
#include "Server.hpp"

extern Server server;

/**
 * @brief Splits the incoming arguments for the USER command.
 *
 * @info This function is used to properly get the realname of the user. The realname can contain spaces, so we need to
 * split the string by spaces first and then join the rest of the arguments to get the realname.
 *
 * @param args The arguments of the USER command. e.g. "username hostname servername :realname"
 * @return vector<string>
 */
static vector<string> user_split(string &args) {
	vector<string> tokens = split(args, ' ');
	string realname;
	for (size_t i = 3; i < tokens.size(); i++) {
		realname += tokens[i];
		if (i + 1 < tokens.size()) {
			realname += " ";
		}
	}
	realname.erase(0, realname.find_first_not_of(':'));
	tokens.erase(tokens.begin() + 3, tokens.end());
	tokens.push_back(realname);
	return tokens;
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
	vector<string> tokens = user_split(args);
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
