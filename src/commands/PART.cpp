#include <stdexcept>

#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

void PART(IRStream &stream, string &args, User *user) {
	// Check if the user is registered
	if (!user->hasHandshake(USER_REGISTERED)) {
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
				throw NotOnChannelException();
			}
			stream.prefix(user).command().param(channel->getName());
			cerr << "stream before end: " << stream.str() << '\n';
			if (parts.size() > 1) {
				stream.param(join(parts, 1)).end();
			} else {
				stream.param("Leaving").end();
			}
			channel->broadcast2(stream, user);
			channel->removeUser(user);
			if (channel->getMembers()->empty()) {
				server.removeChannel(*channel);
			}
		} catch (const IrcException &e) {
			e.e_stream(stream, user);
		}
	}
}
