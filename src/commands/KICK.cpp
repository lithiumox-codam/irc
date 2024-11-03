
#include <stdexcept>

#include "Channel.hpp"
#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

using namespace std;

static void broadcast(IRStream &stream, Channel *channel) {
	for (auto &member : *channel->getMembers()) {
		stream.sendPacket(member.first);
	}
}

string join(const vector<string> &tokens, size_t start) {
	string result;
	for (size_t i = start; i < tokens.size(); ++i) {
		if (i > start) {
			result += " ";
		}
		result += tokens[i];
	}
	return result;
}

void KICK(IRStream &stream, const string &args, User *user) {
	vector<string> tokens = split(args, ' ');

	if (tokens.size() < 2) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}

	try {
		Channel *channel = server.getChannel(tokens[0]);
		if (!channel->hasUser(user)) {
			stream.prefix().code(ERR_NOTONCHANNEL).param(user->getNickname()).trail("You're not in that channel").end();
			return;
		}

		pair<User *, Modes> *target = channel->getMember(server.getUser(tokens[1]));
		if (target == nullptr) {
			stream.prefix().code(ERR_USERNOTINCHANNEL).param(user->getNickname()).trail("User not in channel").end();
			return;
		}

		pair<User *, Modes> *kicker = channel->getMember(user);
		if (!kicker->second.hasModes(M_OPERATOR)) {
			stream.prefix()
				.code(ERR_CHANOPRIVSNEEDED)
				.param(user->getNickname())
				.param(channel->getName())
				.trail("You're not a channel operator")
				.end();
			return;
		}

		IRStream kickStream;

		kickStream.prefix(user).param("KICK").param(channel->getName()).param(target->first->getNickname());
		if (tokens.size() > 2) {
			kickStream.param(join(tokens, 2)).end();
		} else {
			kickStream.param(kicker->first->getNickname()).end();
		}

		broadcast(kickStream, channel);
		channel->removeUser(target->first);

	} catch (const runtime_error &e) {
		if (std::string(e.what()) == "Channel not found") {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
		} else {
			stream.prefix().code(ERR_NOSUCHNICK).param(user->getNickname()).trail("No such nickname").end();
		}
	}
}
