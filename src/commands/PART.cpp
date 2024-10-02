#include "General.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Codes.hpp"
#include "IRStream.hpp"

extern Server server;

bool PART(IRStream &stream, string &args, User *user) {
	vector<string> parts = split(args, ',');
	for (const auto &part : parts) {
		if (part.empty()) {
			stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
			return false;
		}
		try {
			Channel *channel = server.getChannel(part);
			if (!channel->hasUser(user)) {
				stream.prefix().code(ERR_NOTONCHANNEL).param(user->getNickname()).trail("You're not in that channel").end();
				return false;
			}
			channel->removeUser(user);
			stream.prefix(user).command().param(channel->getName()).trail("Left the channel").end();
			if (channel->getMembers()->empty()) {
				server.removeChannel(*channel);
			}
		} catch (runtime_error &e) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
			return false;
		}
	}
	return true;
}
