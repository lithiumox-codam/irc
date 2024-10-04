#include <iostream>
#include <stdexcept>

#include "Channel.hpp"
#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

bool MODE(IRStream &stream, string &args, User *user) {
	// unsigned int added = 0;
	// unsigned int removed = 0;

	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).trail("MODE :Not enough parameters").end();
		return false;
	}

	vector<string> tokens = split(args, ' ');
	for (auto &token : tokens) {
		cout << "Token: " << token << endl;
	}
	cout << tokens.front() << endl;

	if (tokens.front().starts_with("#")) {
		cout << "Channel mode" << endl;
		try {
			auto *channel = server.getChannel(tokens.front());
			if (tokens.size() == 1) {
				channel->modes.addModes(M_MODERATED | M_TOPIC_LOCK);
				stream.prefix()
					.code(RPL_CHANNELMODEIS)
					.param(user->getNickname())
					.param(channel->getName())
					.trail("+" + channel->modes.getModesString())
					.end();
				stream.prefix()
					.code(RPL_CREATIONTIME)
					.param(user->getNickname())
					.param(channel->getName())
					.trail(channel->getCreated())
					.end();
				return true;
			}

		} catch (runtime_error &e) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).trail(tokens.front() + " :No such channel").end();
			return false;
		}
	}
	return true;
}
