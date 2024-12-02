#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

static void sendWhoReply(IRStream& stream, User* requestingUser, Channel* channel, User* member) {
	// Construct basic status (H for here, possibly with channel-specific modifiers)
	string status = "H";

	// Add channel-specific status indicators only if channel is provided
	if (channel != nullptr) {
		auto* channelMember = channel->getMember(member);
		if (channelMember->second.has(M_OPERATOR)) {
			status += "@";
		} else if (channelMember->second.has(M_VOICE)) {
			status += "+";
		}
	}

	if (member->modes.has(M_OPERATOR) && channel == nullptr) {
		status += "*";
	}

	stream.prefix()
		.code(RPL_WHOREPLY)										 // 352
		.param(requestingUser->getNickname())					 // Requesting user's nickname
		.param((channel != nullptr) ? channel->getName() : "*")	 // Channel name or "*" if no channel
		.param(member->getUsername())							 // Username
		.param(member->getHostname())							 // Hostname
		.param(server.getHostname())							 // Servername
		.param(member->getNickname())							 // Nickname
		.param(status)											 // Status (H/G + * @)
		.param("0")												 // Hopcount without a colon
		.trail(member->getRealname())
		.end();
}

void WHO(IRStream& stream, string& args, User* user) {
	if (!user->hasHandshake(H_REGISTERED)) {
		stream.prefix().code(ERR_NOTREGISTERED).trail("You have not registered").end();
		return;
	}

	if (args.empty()) {
		const auto users = server.getUsers();
		for (const auto& usr : users) {
			if (usr.modes.has(M_INVISIBLE)) {
				continue;
			}
			sendWhoReply(stream, user, nullptr, const_cast<User*>(&usr));
		}
		stream.prefix().code("315").param(user->getNickname()).param("*").trail("End of WHO list").end();
		return;
	}

	auto tokens = split(args, ' ');
	if (tokens.size() != 1) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).trail("WHO :Not enough parameters").end();
		return;
	}

	string channelName = tokens[0];
	Channel* channel = server.getChannel(channelName);
	if (channel == nullptr) {
		stream.prefix().code(ERR_NOSUCHCHANNEL).trail("No such channel: " + channelName).end();
		return;
	}

	auto* users = channel->getMembers();
	for (const auto& member : *users) {
		if (member.second.has(M_INVISIBLE)) {
			continue;
		}
		sendWhoReply(stream, user, channel, member.first);
	}

	stream.prefix().code("315").param(user->getNickname()).param(channelName).trail("End of WHO list").end();
}
