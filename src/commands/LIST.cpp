#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

void LIST(IRStream &stream, string &args, User *user) {
	if (!user->hasHandshake(H_REGISTERED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return;
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
		return;
	}

	vector<string> channels = split(args, ',');

	if (channels.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
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
		} catch (const IrcException &e) {
			e.e_stream(stream, user);
		}
	}
}
