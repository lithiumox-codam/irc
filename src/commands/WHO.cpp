#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

static void sendWhoReply(IRStream& stream, User* user, Channel* channel, User* member) {
	stream.prefix()
		.code(RPL_WHOREPLY)							 // 352
		.param(user->getNickname())					 // requesting user's nickname
		.param(channel->getName())					 // channel name with '#'
		.param(member->getUsername())				 // username
		.param(member->getHostname())				 // hostname
		.param(server.getHostname())				 // servername
		.param(member->getNickname())				 // nickname
		.params({"H", channel->getUserModes(user)})	 // status (H/G + * @ +)
		.param("0")									 // hopcount without a colon
		.trail(member->getRealname())
		.end();
}

void WHO(IRStream& stream, const string& args, User* user) {
	if (!user->hasHandshake(USER_REGISTERED)) {
		stream.prefix().code(ERR_NOTREGISTERED).trail("You have not registered").end();
		return;
	}
	if (args.empty()) {
		const auto& users = server.getUsers();
		for (const auto& usr : users) {
			stream.prefix()
				.code(RPL_WHOREPLY)			  // 352
				.param(user->getNickname())	  // requesting user's nickname
				.param("*")					  // channel name with '*'
				.param(usr.getUsername())	  // username
				.param(usr.getHostname())	  // hostname
				.param(server.getHostname())  // servername
				.param(usr.getNickname())	  // nickname
				.param("H+")				  // status (H/G + * @ +)
				.param("0")					  // hopcount without a colon
				.trail(usr.getRealname())
				.end();
		}
		stream.prefix()
			.code("315")				 // 315
			.param(user->getNickname())	 // requesting user's nickname
			.param("*")					 // channel name
			.trail("End of WHO list")
			.end();
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
		if (member.second.hasModes(M_INVISIBLE)) {
			continue;
		}
		sendWhoReply(stream, user, channel, member.first);
	}
	stream.prefix().code("315").param(user->getNickname()).param(channelName).trail("End of WHO list").end();
}
