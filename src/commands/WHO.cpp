#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

bool WHO(IRStream& stream, string& args, User* user) {
	auto tokens = split(args, ' ');
	if (tokens.size() != 1) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).trail("WHO :Not enough parameters").end();
		return false;
	}
	string channelName = tokens[0];
	Channel* channel = server.getChannel(channelName);
	if (channel == nullptr) {
		stream.prefix().code(ERR_NOSUCHCHANNEL).trail("No such channel: " + channelName).end();
		return false;
	}
	auto* users = channel->getMembers();
	for (const auto& member : *users) {
		stream.prefix()
			.code(RPL_WHOREPLY)					 // 352
			.param(user->getNickname())			 // requesting user's nickname
			.param(channel->getName())			 // channel name with '#'
			.param(member.first->getUsername())	 // username
			.param(member.first->getHostname())	 // hostname
			.param(server.getHostname())		 // servername
			.param(member.first->getNickname())	 // nickname
			.param("H+")						 // status (H/G + * @ +)
			.param("0")							 // hopcount without a colon
			.trail(member.first->getRealname())	 // realname (handled with colon automatically by trail)
			.end();
	}
	stream.prefix()
		.code("315")				 // 315
		.param(user->getNickname())	 // requesting user's nickname
		.param(channelName)			 // channel name
		.trail("End of WHO list")
		.end();
	return true;
}
