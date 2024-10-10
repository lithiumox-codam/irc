#include <iostream>
#include <stdexcept>
#include <string>

#include "Channel.hpp"
#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

void applyModeChanges(Modes &modes, string &modeChanges) {
	const std::string modeChars = "ovimiktl";
	const unsigned int modeValues[] = {M_OPERATOR,	  M_VOICE,	  M_INVISIBLE,	M_MODERATED,
									   M_INVITE_ONLY, M_PASSWORD, M_TOPIC_LOCK, M_LIMIT};
	for (size_t i = 0; i < modeChanges.size(); ++i) {
		if (modeChanges[i] == '+') {
			modes.addModes(modeValues[modeChars.find(modeChanges[i + 1])]);
		} else if (modeChanges[i] == '-') {
			modes.removeModes(modeValues[modeChars.find(modeChanges[i + 1])]);
		}
	}
}

bool MODE(IRStream &stream, string &args, User *user) {
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
		try {
			auto *channel = server.getChannel(tokens.front());
			if (tokens.size() == 1) {
				stream.prefix()
					.code(RPL_CHANNELMODEIS)
					.param(user->getNickname())
					.param(channel->getName())
					.trail("+" + channel->modes.getModesString())
					.end();
				return true;
			}
			if (tokens.size() == 2) {
				applyModeChanges(channel->modes, tokens[1]);
				stream.prefix()
					.code(RPL_CHANNELMODEIS)
					.param(user->getNickname())
					.param(channel->getName())
					.trail("+" + channel->modes.getModesString())
					.end();
				return true;
			}
			if (tokens.size() == 3) {
				try {
					auto &member = channel->getMember(tokens[2]);
					applyModeChanges(member.second, tokens[1]);

				} catch (runtime_error &e) {
					string error = e.what();
					stream.prefix().code(ERR_NOSUCHNICK).trail(error).end();
					return false;
				}
			}

		} catch (runtime_error &e) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).trail(tokens.front() + " :No such channel").end();
			return false;
		}
	}
	return true;
}
