#include "Codes.hpp"
#include "Exceptions.hpp"
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

	IRStream updateStream;
	updateStream.prefix(user);

	try {
		user->setNickname(args);
	} catch (const IrcException &e) {
		e.e_stream(stream, user);
		return;
	}

	if (user->hasHandshake(H_NICK)) {
		stream.prefix().command().param(user->getNickname()).end();
		updateStream.param("NICK").param(user->getNickname()).end();
		for (auto &channel : server.getChannels()) {
			if (channel.hasUser(user)) {
				channel.broadcast(updateStream);
			}
		}
	} else {
		user->addHandshake(H_NICK);
	}

	if (user->hasHandshake(H_AUTHENTICATED) && !user->hasHandshake(H_WELCOME)) {
		string empty;
		MOTD(stream, empty, user);
		user->addHandshake(H_WELCOME);
	}
}
