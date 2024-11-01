#include <iostream>
#include <stdexcept>
#include <string>

#include "Channel.hpp"
#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

void applyModeChanges(Modes &modes, string &modeChanges) {
	const string modeChars = "ovimktl";
	const unsigned int modeValues[] = {M_OPERATOR, M_VOICE,		 M_INVISIBLE, M_MODERATED,
									   M_PASSWORD, M_TOPIC_LOCK, M_LIMIT};
	for (size_t i = 0; i < modeChanges.size(); ++i) {
		if (modeChanges[i] == '+' || modeChanges[i] == '-') {
			bool addMode = (modeChanges[i] == '+');
			unsigned int modeToApply = 0;

			if (modeChanges[i + 1] == 'i') {
				if (modes.getType() == Type::CHANNEL) {
					modeToApply = M_INVITE_ONLY;
				} else if (modes.getType() == Type::USER) {
					modeToApply = M_INVISIBLE;
				}
			} else {
				modeToApply = modeValues[modeChars.find(modeChanges[i + 1])];
			}

			if (addMode) {
				modes.addModes(modeToApply);
			} else {
				modes.removeModes(modeToApply);
			}
			++i;
		}
	}
}

// static void handleChannel(IRStream &stream, User *user, string &channelName, string &modeChanges) {
// 	try {
// 		auto *channel = server.getChannel(channelName);
// 		if (modeChanges.size() == 1) {
// 			stream.prefix()
// 				.code(RPL_CHANNELMODEIS)
// 				.param(user->getNickname())
// 				.param(channel->getName())
// 				.trail("+" + channel->modes.getModesString())
// 				.end();
// 			return;
// 		}
// 		applyModeChanges(channel->modes, modeChanges);
// 		IRStream broadcast;
// 		broadcast.prefix()
// 			.param(user->getNickname())
// 			.code(RPL_CHANNELMODEIS)
// 			.param(channel->getName())
// 			.trail("+" + channel->modes.getModesString())
// 			.end();
// 		channel->broadcast(user, broadcast.getString());
// 	} catch (runtime_error &e) {
// 		stream.prefix().code(ERR_NOSUCHCHANNEL).trail(channelName + " :No such channel").end();
// 	}
// }

bool MODE(IRStream &stream, string &args, User *user) {
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).trail("MODE :Not enough parameters").end();
		return false;
	}

	vector<string> tokens = split(args, ' ');

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
				if (tokens[1].starts_with("+") || tokens[1].starts_with("-")) {
					applyModeChanges(channel->modes, tokens[1]);
					stream.prefix()
						.code(RPL_CHANNELMODEIS)
						.param(user->getNickname())
						.param(channel->getName())
						.trail("+" + channel->modes.getModesString())
						.end();
					return true;
				}
				try {
					auto &member = channel->getMember(tokens[1]);
					stream.prefix()
						.code(RPL_UMODEIS)
						.param(user->getNickname())
						.trail(member.first->getNickname() + " User modes: +" + member.second.getModesString())
						.end();
				} catch (runtime_error &e) {
					string error = e.what();
					stream.prefix().code(ERR_NOSUCHNICK).trail(error).end();
					return false;
				}
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

		} catch (IrcException &e) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).trail(tokens.front() + " :No such channel").end();
			return false;
		}
	}

	return true;
}
