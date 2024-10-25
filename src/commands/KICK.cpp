
#include <stdexcept>
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Modes.hpp"
#include "Codes.hpp"

extern Server server;

using namespace std;

string join(const vector<string>& tokens, size_t start) {
	string result;
	for (size_t i = start; i < tokens.size(); ++i) {
		if (i > start) {
			result += " ";
		}
		result += tokens[i];
	}
	return result;
}

bool KICK(IRStream &stream, string &args, User *user){
	vector<string> tokens = split(args, ' ');
	if (tokens.size() < 2) {
		stream.prefix()
		.code(ERR_NEEDMOREPARAMS)
		.param(user->getNickname())
		.trail("Not enough parameters")
		.end();
		return false;
	}
	try {
		Channel *channel = server.getChannel(tokens[0]);
		if (!channel->hasUser(user)) {
			stream.prefix()
			.code(ERR_NOTONCHANNEL)
			.param(user->getNickname())
			.trail("You're not in that channel")
			.end();
			return false;
		}
		pair<User *, Modes> *target = channel->getMember(server.getUser(tokens[1]));
		if (target == nullptr){
			stream.prefix()
			.code(ERR_USERNOTINCHANNEL)
			.param(user->getNickname())
			.trail("User not in channel")
			.end();
			return false;
		}
		pair<User *, Modes> *kicker = channel->getMember(user);
		if (!kicker->second.hasModes(M_OPERATOR)){
			stream.prefix()
			.code(ERR_CHANOPRIVSNEEDED)
			.param(user->getNickname())
			.trail("You're not a channel operator")
			.end();
			return false;
		}
		channel->removeUser(target->first);
		stream.prefix(user)
			.command()
			.param(channel->getName())
			.param(target->first->getNickname());
		if (tokens.size() > 2) {
			stream.param(join(tokens, 2));
		} else {
			stream.param(kicker->first->getNickname());
		}
		stream.end();
		return true;
	} catch (const runtime_error &e) {
		if (std::string(e.what()) == "Channel not found") {
			stream.prefix()
			.code(ERR_NOSUCHCHANNEL)
			.param(user->getNickname())
			.trail("No such channel")
			.end();
			return false;
		}
		stream.prefix()
		.code(ERR_NOSUCHNICK)
		.param(user->getNickname())
		.trail("No such nickname")
		.end();
		return false;
	}
}

