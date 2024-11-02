#include <algorithm>
#include <vector>

#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

void NICK(IRStream &stream, string &args, User *user) {
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).trail("Not enough parameters").end();
		return ;
	}

	const auto &users = server.getUsers();

	auto iter = find_if(users.begin(), users.end(), [&args](const auto &user) { return user.getNickname() == args; });

	if (iter != users.end()) {
		stream.prefix().code(ERR_NICKNAMEINUSE).param(user->getUsername()).param(args).param(args).param(" is already in use").end();
		return ;
	}

	user->setNickname(args);
	user->addHandshake(USER_NICK);

	stream.prefix().command().param(user->getNickname()).end();

	if (user->hasHandshake(USER_AUTHENTICATED) &&
		!user->hasHandshake(USER_WELCOME)) {
		string empty;
		MOTD(stream, empty, user);
		user->addHandshake(USER_WELCOME);
	}
}
