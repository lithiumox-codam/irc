#include <sstream>
#include <stdexcept>

#include "Channel.hpp"
#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;
using namespace std;

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
	if (!user->hasHandshake(U_COMPLETED)) {
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
		if (token.first.size() > 50) {
			stream.prefix().code(ERR_NOSUCHCHANNEL).param(user->getNickname()).trail("No such channel").end();
			return false;
		}
		try {
			Channel *channel = server.getChannel(token.first);
			if (!token.second.empty() && channel->hasModes(M_PASSWORD) && channel->getPassword() != token.second) {
				stream.prefix()
					.code(ERR_BADCHANNELKEY)
					.param(user->getNickname())
					.param(token.first)
					.trail("Cannot join channel (+k) - bad key")
					.end();
				return false;
			}
			channel->addUser(user);
			stream.prefix(user).command().param(channel->getName()).end();
		} catch (const runtime_error &e) {
			server.addChannel(token.first);
			Channel *channel = server.getChannel(token.first);
			channel->addUser(user);
			if (!token.second.empty()) {
				channel->setPassword(token.second);
				channel->addModes(M_PASSWORD);
			}
			channel->getMembers()->front().second.addModes(M_OPERATOR);
			stream.prefix(user).command().param(channel->getName()).end();
		}
	}
	return true;
}

// bool JOIN(stringstream &stream, string &args, User &user) {

// 	if (!user.hasHandshake(U_COMPLETED)) {
// 		stream << startRes(ERR_NOTREGISTERED) << user.getNickname() << " :You have not registered" << END;
// 		return false;
// 	}
// 	if (args.empty()) {
// 		stream << startRes(ERR_NEEDMOREPARAMS) << user.getNickname() << " JOIN :Not enough parameters" << END;
// 		return false;
// 	}
// 	map<string, string> tokens = ParseJoin(args);
// 	for (auto &token : tokens) {
// 		if (token.first.empty()) {
// 			stream << startRes(ERR_NEEDMOREPARAMS) << user.getNickname() << " JOIN :Not enough parameters" << END;
// 			return false;
// 		}
// 		if (token.first[0] != '#') {
// 			stream << startRes(ERR_NOSUCHCHANNEL) << user.getNickname() << " " << token.first << " :No such channel" <<
// END; 			return false;
// 		}
// 		if (token.first.size() > 50) {
// 			stream << startRes(ERR_NOSUCHCHANNEL) << user.getNickname() << " " << token.first << " :No such channel" <<
// END; 			return false;
// 		}
// 		try {
// 			if (!token.second.empty() && server.getChannel(token.first).hasModes(M_PASSWORD) &&
// server.getChannel(token.first).getPassword() != token.second) { 				stream << startRes(ERR_BADCHANNELKEY) <<
// user.getNickname() << " " << token.first << " :Cannot join channel (+k)" << END; 				return false;
// 			}
// 			server.getChannel(token.first).addUser(user);
// 		} catch (const runtime_error &e) {
// 			server.addChannel(token.first);
// 			server.getChannel(token.first).addUser(user);
// 			if (!token.second.empty()) {
// 				server.getChannel(token.first).setPassword(token.second);
// 				server.getChannel(token.first).addModes(M_PASSWORD);
// 			}
// 			server.getChannel(token.first).getMembers().front().addModes(M_OPERATOR);
// 		}
// 		stream << ":" << user.getNickname() << " JOIN " << token.first << END;
// 	}
// 	return true;
// }
