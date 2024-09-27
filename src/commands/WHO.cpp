#include "ChannelMember.hpp"
#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

bool WHO(IRStream& stream, string& args, User& user) {
	vector<string> tokens = split(args, ' ');
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
	vector<ChannelMember>* users = channel->getMembers();
	for (const ChannelMember& member : *users) {
		stream.prefix()
			.code(RPL_WHOREPLY)			  // 352
			.param(user.getNickname())	  // requesting user's nickname
			.param(channel->getName())	  // channel name with '#'
			.param(member.getUsername())  // username
			.param(member.getHostname())  // hostname
			.param(server.getHostname())  // servername
			.param(member.getNickname())  // nickname
			.param("H+")				  // status (H/G + * @ +)
			.param("0")					  // hopcount without a colon
			.trail(member.getRealname())  // realname (handled with colon automatically by trail)
			.end();
	}
	stream.prefix()
		.code("315")				// 315
		.param(user.getNickname())	// requesting user's nickname
		.param(channelName)			// channel name
		.trail("End of WHO list")
		.end();
	return true;
}
