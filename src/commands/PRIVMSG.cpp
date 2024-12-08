#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

using namespace std;

void handleUserMessage(IRStream &stream, const pair<string, string> &token, User *user) {
	if (token.first == user->getNickname()) {
		stream.prefix()
			.code(ERR_NOSUCHNICK)
			.param(user->getNickname())
			.trail("Error: Talking to yourself is not allowed!")
			.end();
		return;
	}

	try {
		IRStream targetStream;
		User *target = server.getUser(token.first);
		targetStream.prefix(user).param("PRIVMSG").param(token.first).trail(token.second).end().sendPacket(target);
	} catch (const runtime_error &e) {
		stream.prefix().code(ERR_NOSUCHNICK).param(user->getNickname()).trail("No such nick/channel").end();
	}
}

void handleChannelMessage(IRStream &stream, const pair<string, string> &token, User *user) {
	try {
		Channel *channel = server.getChannel(token.first);

		if (!channel->hasUser(user)) {
			stream.prefix()
				.code(ERR_NOTONCHANNEL)
				.param(user->getNickname())
				.param(token.first)
				.trail("You're not on that channel")
				.end();
			return;
		}

		if (!channel->modes.hasModes(M_MODERATED)) {
			channel->broadcast(user, token.second);
		} else {
			if (channel->getMember(user->getNickname()).second.hasModes(M_VOICE)) {
				channel->broadcast(user, token.second);
			} else if (channel->hasOperator(user)) {
				channel->broadcast(user, token.second);
			} else {
				stream.prefix()
					.code(ERR_CANNOTSENDTOCHAN)
					.param(user->getNickname())
					.param(token.first)
					.trail("Cannot send to channel missing voice! (+m)")
					.end();
				return;
			}
		}
	} catch (const runtime_error &e) {
		stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
	}
}

void PRIVMSG(IRStream &stream, string &args, User *user) {
	if (args.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}

	pair<string, string> token = splitPair(args, ' ');

	if (token.first.empty() || token.second.empty()) {
		stream.prefix().code(ERR_NEEDMOREPARAMS).param(user->getNickname()).trail("Not enough parameters").end();
		return;
	}

	if (token.first[0] != '#') {
		handleUserMessage(stream, token, user);
	} else {
		handleChannelMessage(stream, token, user);
	}
}
