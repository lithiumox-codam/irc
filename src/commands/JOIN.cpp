#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "Channel.hpp"
#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

static void broadcast(Channel *channel, User *user) {
	IRStream stream;

	stream.setCommand("JOIN");

	stream.prefix(user).command().trail(channel->getName()).end();
	for (auto &member : *channel->getMembers()) {
		if (member.first->getSocket() != user->getSocket()) {
			stream.sendPacket(member.first);
		}
	}
}

static void addToChannel(IRStream &stream, Channel *channel, User *user) {
	channel->addUser(user);

	stream.prefix(user).command().param(channel->getName()).end();
	if (channel->getTopic().empty()) {
		stream.prefix().code(RPL_NOTOPIC).param(channel->getName()).trail("No topic is set").end();
	} else {
		stream.prefix()
			.code(RPL_TOPIC)
			.param(user->getNickname())
			.param(channel->getName())
			.trail(channel->getTopic())
			.end();
	}

	// Send the names list
	stream.prefix().code(RPL_NAMREPLY).param(user->getNickname()).param("=").param(channel->getName()).trail("");
	for (auto &member : *channel->getMembers()) {
		stream.param(member.first->getNickname());
	}
	stream.end();

	stream.prefix()
		.code(RPL_ENDOFNAMES)
		.param(user->getNickname())
		.param(channel->getName())
		.trail("End of /NAMES list")
		.end();

	broadcast(channel, user);
}

void JOIN(IRStream &stream, const string &args, User *user) {
	// Check if the user is registered
	if (!user->hasHandshake(USER_REGISTERED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return;
	}
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}

	// Parse the channels and passwords
	vector<string> parts = split(args, ' ');

	if (parts.empty() || parts.size() > 2) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}

	vector<string> channelNames = split(parts[0], ',');
	vector<string> passwords = (parts.size() > 1) ? split(parts[1], ',') : vector<string>();
	auto password = passwords.begin();

	if (channelNames.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}

	// Try joining the channels
	for (string &channelName : channelNames) {
		if (channelName[0] != '#') {
			stream.prefix()
				.code(ERR_NOSUCHCHANNEL)
				.param(user->getNickname())
				.param(channelName)
				.trail("No such channel")
				.end();	 // Channel names must start with #
			continue;
		}

		try {
			Channel *channel = server.getChannel(channelName);

			if (channel->hasUser(user)) {
				stream.prefix()
					.code(ERR_USERONCHANNEL)
					.param(user->getNickname())
					.param(channel->getName())
					.trail("You're already on that channel")
					.end();
				continue;
			}
			if (channel->getMembers()->size() >= MEMBER_LIMIT) {
				stream.prefix()
					.code(ERR_CHANNELISFULL)
					.param(user->getNickname())
					.param(channel->getName())
					.trail("Channel is full")
					.end();
				continue;
			}

			if (channel->hasInvited(user)) {
				channel->removeInvited(user);
			} else if (channel->modes.hasModes(M_INVITE_ONLY)) {
				stream.prefix()
					.code(ERR_INVITEONLYCHAN)
					.param(user->getNickname())
					.param(channel->getName())
					.trail("Cannot join channel (+i)")
					.end();
				continue;
			}

			if (channel->modes.hasModes(M_PASSWORD)) {
				if (password == passwords.end()) {
					stream.prefix()
						.code(ERR_BADCHANNELKEY)
						.param(user->getNickname())
						.param(channelName)
						.trail("Cannot join channel (+k) - bad key")
						.end();
					continue;
				}
				if (*password != channel->getPassword()) {
					stream.prefix()
						.code(ERR_BADCHANNELKEY)
						.param(user->getNickname())
						.param(channelName)
						.trail("Cannot join channel (+k) - bad key")
						.end();
					password++;
					continue;
				}
				password++;
			}

			addToChannel(stream, channel, user);

		} catch (const runtime_error &e) {	// Channel not found
			server.addChannel(
				channelName);  // We're not running any checks on the channel name, maybe alpha-numeric only?
			Channel *channel = server.getChannel(channelName);
			channel->addOperator(user);
			channel->addUser(user);

			if (password != passwords.end()) {
				channel->setPassword(*password);
				channel->modes.addModes(M_PASSWORD);
				password++;
			}

			stream.prefix(user).command().param(channel->getName()).end();
		}
	}
}
