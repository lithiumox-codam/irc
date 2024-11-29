#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "Channel.hpp"
#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

static void sendChannelTopic(IRStream &stream, Channel *channel, User *user) {
	if (!channel->getTopic().empty()) {
		stream.prefix()
			.code(RPL_TOPIC)
			.param(user->getNickname())
			.param(channel->getName())
			.trail(channel->getTopic())
			.end();
		stream.prefix()
			.code(RPL_TOPICTIME)
			.param(user->getNickname())
			.param(channel->getName())
			.param(channel->getTopicSetter())
			.trail(to_string(channel->getTopicTime()))
			.end();
	}
}

static void makeNamesList(IRStream &stream, Channel *channel, User *user) {
	stream.prefix().code(RPL_NAMREPLY).param(user->getNickname()).param("=").param(channel->getName()).trail("");
	for (auto &member : *channel->getMembers()) {
		if (!member.second.hasModes(M_INVISIBLE)) {
			stream.param((member.second.hasModes(M_OPERATOR) ? "@" : "") + member.first->getNickname());
		}
	}
	stream.end();
	stream.prefix()
		.code(RPL_ENDOFNAMES)
		.param(user->getNickname())
		.param(channel->getName())
		.trail("End of /NAMES list")
		.end();
}

static void channelChecks(Channel *channel, User *user) {
	if (channel->hasUser(user)) {
		throw UserAlreadyOnChannelException();
	}
	if (channel->getMembers()->size() >= MEMBER_LIMIT) {
		throw ChannelFullException();
	}

	if (channel->hasInvited(user)) {
		channel->removeInvited(user);
	} else if (channel->modes.hasModes(M_INVITE_ONLY)) {
		throw InviteOnlyChannelException();
	}
}

void JOIN(IRStream &stream, string &args, User *user) {
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
	// Try joining the channels
	for (string &channelName : channelNames) {
		if (channelName[0] != '#') {
			NoSuchChannelException().e_stream(stream, user);
			continue;
		}
		try {
			Channel *channel = server.getChannel(channelName);

			channelChecks(channel, user);
			if (channel->modes.hasModes(M_PASSWORD)) {
				if (password == passwords.end()) {
					throw BadChannelKeyException();
				}
				if (*password != channel->getPassword()) {
					password++;
					throw BadChannelKeyException();
				}
				password++;
			}
			channel->addUser(user);
			stream.prefix(user).command().param(channel->getName()).end();
			channel->broadcast(stream, user);
			sendChannelTopic(stream, channel, user);
			makeNamesList(stream, channel, user);
		} catch (const IrcException &e) {
			if (e.GetCode() != ERR_NOSUCHCHANNEL) {
				e.e_stream(stream, user);
				continue;
			}
			server.addChannel(channelName);
			Channel *channel = server.getChannel(channelName);
			channel->addOperator(user);
			channel->addUser(user);

			if (password != passwords.end()) {
				channel->setPassword(*password);
				channel->modes.addModes(M_PASSWORD);
				password++;
			}
			stream.prefix(user).command().param(channel->getName()).end();
			makeNamesList(stream, channel, user);
		}
	}
}
