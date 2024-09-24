#include <stdexcept>
#include "General.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "ChannelMember.hpp"
#include <iostream>

extern Server server;
using namespace std;

bool JOIN(stringstream &stream, string &args, User &user) {
	
	if (!user.hasHandshake(U_COMPLETED)) {
		stream << startRes(ERR_NOTREGISTERED) << user.getNickname() << " :You have not registered" << END;
		return false;
	}
	if (args.empty()) {
		stream << startRes(ERR_NEEDMOREPARAMS) << user.getNickname() << " JOIN :Not enough parameters" << END;
		return false;
	}
	cerr << args << "\n";
	vector<string> tokens = split(args, ' ');
	cerr << tokens.size() << "\n";
	if (tokens.size() < 1) {
		stream << startRes(ERR_NEEDMOREPARAMS) << user.getNickname() << " JOIN :Not enough parameters" << END;
		return false;
	}
	string channel = tokens[0];
	if (channel[0] != '#') {
		stream << startRes(ERR_NOSUCHCHANNEL) << user.getNickname() << " " << channel << " :No such channel" << END;
		return false;
	}
	if (channel.size() > 50) {
		stream << startRes(ERR_NOSUCHCHANNEL) << user.getNickname() << " " << channel << " :No such channel" << END;
		return false;
	}
	Channel newChannel;
	try {
		newChannel = server.getChannel(channel);
		newChannel.addUser(user);
	} catch (const runtime_error &e) {
		newChannel = server.addChannel(channel);
		newChannel.addUser(user);
		newChannel.getMembers().front().addModes(M_OPERATOR);
	}
	stream << ":" << user.getNickname() << " JOIN " << channel << END;
	return true;
}
