#include <sstream>
#include <stdexcept>

#include "Channel.hpp"
#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;
using namespace std;

static void		addToChannel(IRStream &stream, Channel *channel, User *user) {
	channel->addUser(user);

	stream.prefix(user).command().param(channel->getName()).end();
	if (channel->getTopic().empty()) {
		stream.prefix().code(RPL_NOTOPIC).param(channel->getName()).trail("No topic is set").end();
	} else {
		stream.prefix().code(RPL_TOPIC).param(user->getNickname()).param(channel->getName()).trail(channel->getTopic()).end();
	}

	// Send the names list
	string		namesList;
	for (auto &member : *channel->getMembers()) {
		namesList += member.first->getNickname() + " ";
	}

	stream.prefix().code(RPL_NAMREPLY).param(user->getNickname()).param("=").param(channel->getName()).trail(namesList).end();
	stream.prefix().code(RPL_ENDOFNAMES).param(user->getNickname()).param(channel->getName()).trail("End of /NAMES list").end();
}

map<string, string> ParseJoin(string &args) {
	map<string, string> channelPasswordMap;

	stringstream sstr(args);
	string channelsPart;
	string passwordsPart;

	getline(sstr, channelsPart, ' ');
	getline(sstr, passwordsPart);

	vector<string> channels = split(channelsPart, ',');
	vector<string> passwords = split(passwordsPart, ',');

	for (size_t i = 0; i < channels.size(); ++i) {
		if (i < passwords.size()) {
			channelPasswordMap[channels[i]] = passwords[i];
		} else {
			channelPasswordMap[channels[i]] = "";
		}
	}
	return channelPasswordMap;
}

bool JOIN(IRStream &stream, string &args, User *user) {
	if (!user->hasHandshake(USER_REGISTERED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return false;
	}
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return false;
	}
	map<string, string> tokens = ParseJoin(args);
	for (auto &token : tokens) {
		if (token.first.empty()) {
			stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
			return false;
		}
		if (token.first[0] != '#') {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
			return false;
		}
		if (token.first.size() >= CHANNEL_LIMIT) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
			return false;
		}
		try {
			Channel *channel = server.getChannel(token.first);
			if (channel->getMembers()->size() >= CHANNEL_LIMIT) {
				stream.prefix().code(ERR_CHANNELISFULL).param(user->getNickname()).trail("Channel is full").end();
				return false;
			}
			if (channel->hasInvited(user)) {
				channel->removeInvited(user);
			} else if (channel->modes.hasModes(M_INVITE_ONLY)) {
				stream.prefix().code(ERR_INVITEONLYCHAN).param(user->getNickname()).trail("Cannot join channel (+i)").end();
				return false;
			} else if (channel->modes.hasModes(M_PASSWORD) && channel->getPassword() != token.second) {
				stream.prefix()
					.code(ERR_BADCHANNELKEY)
					.param(user->getNickname())
					.param(token.first)
					.trail("Cannot join channel (+k) - bad key")
					.end();
				return false;
			}

			addToChannel(stream, channel, user);

		} catch (const runtime_error &e) { // Channel not found
			server.addChannel(token.first);
			Channel *channel = server.getChannel(token.first);
			channel->addOperator(user);
			channel->addUser(user);
			if (!token.second.empty()) {
				channel->setPassword(token.second);
				channel->modes.addModes(M_PASSWORD);
			}
			stream.prefix(user).command().param(channel->getName()).end();
		}
	}
	return true;
}
