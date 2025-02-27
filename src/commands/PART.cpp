#include <algorithm>

#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

void PART(IRStream &stream, string &args, User *user) {
	// Check if the user is registered
	if (!user->hasHandshake(H_REGISTERED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return;
	}

	// Parse the channels and passwords
	vector<string> parts = split(args, ' ');

	if (parts.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}

	vector<string> channels = split(parts[0], ',');

	// Try leaving the channels
	for (const auto &channelName : channels) {
		try {
			Channel *channel = server.getChannel(channelName);
			if (!channel->hasUser(user)) {
				throw NotOnChannelException(user->getNickname());
			}
			stream.prefix(user).command().param(channel->getName());
			if (parts.size() > 1) {
				for_each(parts.begin() + 1, parts.end(), [&stream](const string &str) { stream.param(str); });
				stream.end();
			} else {
				stream.param("Leaving").end();
			}
			channel->broadcast(stream, user);
			channel->removeUser(user);
			if (channel->getMembers()->empty()) {
				server.removeChannel(*channel);
			}
		} catch (const IrcException &e) {
			e.e_stream(stream, user);
		}
	}
}
