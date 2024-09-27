#include <algorithm>
#include <iostream>
#include <vector>

#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

bool NICK(IRStream &stream, string &args, User &user) {
	if (args.empty()) {
		stream.code(ERR_NEEDMOREPARAMS).trail("Not enough parameters").end();
		return false;
	}

	const vector<User> &users = server.getUsers();

	auto iter = find_if(users.begin(), users.end(), [&args](const User &user) { return user.getNickname() == args; });

	if (iter != users.end()) {
		stream.code(ERR_NICKNAMEINUSE).param(user.getNickname()).param(args).trail("Nickname is already in use").end();
		return false;
	}

	user.setNickname(args);
	user.addHandshake(U_NICK);

	stream.prefix().command().param(user.getNickname()).end();
	return true;
}
