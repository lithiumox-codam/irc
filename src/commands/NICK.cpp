#include <algorithm>
#include <vector>

#include "Codes.hpp"
#include "General.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

bool NICK(stringstream &stream, string &args, User &user) {
	if (args.empty()) {
		stream.str("");
		stream << startRes(ERR_NEEDMOREPARAMS) + user.getNickname() + " NICK :Not enough parameters" << END;
		return false;
	}

	const vector<User> &users = server.getUsers();

	auto iter = find_if(users.begin(), users.end(), [&args](const User &user) { return user.getNickname() == args; });

	if (iter != users.end()) {
		stream.str("");
		stream << startRes(ERR_NICKNAMEINUSE) + user.getNickname() + " " + args + " :Nickname is already in use" << END;
		return false;
	}

	user.setNickname(args);
	user.addHandshake(U_NICK);
	stream << ":" << user.getNickname() << " NICK " << user.getNickname() << END;
	return true;
}
