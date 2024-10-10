#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

bool LIST(IRStream &stream, string &args, User *user) {
	if (!user->hasHandshake(U_COMPLETED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return false;
	}
	if (args.empty()) {
		for (auto &channel : server.getChannels()) {
			stream.prefix()
				.code(RPL_LIST)
				.param(user->getNickname())
				.param(channel.getName())
				.param(to_string(channel.getMembers()->size()))
				.trail(channel.getTopic())
				.end();
		}
		stream.prefix().code(RPL_LISTEND).param(user->getNickname()).trail("End of LIST").end();
		return true;
	}

	vector<string> channels = split(args, ',');

	if (channels.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return false;
	}

	for (const auto &channelName : channels) {
		try {
			Channel *channel = server.getChannel(channelName);
			stream.prefix()
				.code(RPL_LIST)
				.param(user->getNickname())
				.param(channel->getName())
				.param(to_string(channel->getMembers()->size()))
				.trail(channel->getTopic())
				.end();
		} catch (runtime_error &e) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail(e.what()).end();
			return false;
		}
	}
	return true;
}
