#include <stdexcept>
#include "Exceptions.hpp"
#include "General.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Codes.hpp"
#include "IRStream.hpp"

extern Server server;

static void	broadcast(Channel *channel, User *user, const string &message) {
	IRStream stream;

	stream.setCommand("PART");

	stream.prefix(user).command().param(channel->getName()).trail(message).end();
	for (auto &member : *channel->getMembers()) {
		if (member.first->getSocket() != user->getSocket()) {
			stream.sendPacket(member.first);
		}
	}
}

bool PART(IRStream &stream, string &args, User *user) {
	// Check if the user is registered
	if (!user->hasHandshake(USER_REGISTERED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return false;
	}

	// Parse the channels and passwords
	vector<string>	parts = split(args, ' ');

	if (parts.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return false;
	}

	vector<string>	channels = split(parts[0], ',');
	string			message;
	if (parts.size() > 1) {
		if (parts[1].starts_with(':')) {
			message = parts[1].substr(1);
		} else {
			stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
			return false;
		}
		message = parts[1].substr(1);
	} else {
		message = "Leaving";
	}

	// Try leaving the channels
	for (const auto &channelName : channels) {
		try {
		Channel *channel = server.getChannel(channelName);
		if (!channel->hasUser(user)) {
			stream.prefix().code(ERR_NOTONCHANNEL).param(user->getNickname()).param(channelName).trail("You're not in that channel").end();
			continue ;
		}

		channel->removeUser(user);

		if (channel->getMembers()->empty()) {
			server.removeChannel(*channel);
		} else {
			broadcast(channel, user, message);
		}

		} catch (runtime_error &e) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
			continue ;
		}
	}
	return true;
}
