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
		return;
	}

	const auto &users = server.getUsers();

	auto iter = find_if(users.begin(), users.end(), [&args](const auto &user) { return user.getNickname() == args; });

	if (iter != users.end()) {
		stream.prefix()
			.code(ERR_NICKNAMEINUSE)
			.param(user->getUsername())
			.param(user->getNickname())
			.param(args)
			.param(" is already in use")
			.end();
		return;
	}

	if (user->hasHandshake(H_NICK)) {
		for (auto &channel : server.getChannels()) {
			IRStream serverStream;
			serverStream.prefix(user).param("NICK").trail(args).end();
			if (channel.hasUser(user)) {
				channel.broadcast(serverStream);
			}
		}
	}

	user->setNickname(args);
	user->addHandshake(H_NICK);

	stream.prefix().command().param(user->getNickname()).end();

	if (user->hasHandshake(H_AUTHENTICATED) && !user->hasHandshake(H_WELCOME)) {
		string empty;
		MOTD(stream, empty, user);
		user->addHandshake(H_WELCOME);
	}
}
