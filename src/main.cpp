#include <signal.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "Channel.hpp"
#include "ChannelMember.hpp"
#include "Server.hpp"

Server server;

void signalHandler(int signum) {
	switch (signum) {
		case SIGINT:  // ctrl + c
			server.stop();
			break;
		case SIGTERM:  // kill command
			server.stop();
			break;
		case SIGKILL:  // kill -9 command
			server.stop();
			break;
		default:
			break;
	}
	exit(signum);
}

int main(int argc, char **argv) {
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	if (argc != 3) {
		std::cerr << "Usage: ./ircserver [port] [password]" << std::endl;
		return 1;
	}

	User mees = User("Mees", "localhost", "Joe Mama", -1);
	User kees = User("Kees", "localhost", "Joe Mama", -1);
	Channel channel = Channel("General");
	channel.addUser(mees);
	channel.addUser(kees);

	channel.addModes(M_MODERATED | M_INVITE_ONLY | M_PASSWORD);
	channel.removeModes(M_INVITE_ONLY);
	channel.printModes();

	for (auto &member : channel.getMembers()) {
		if (member.getUsername() == "Mees") {
			member.setModes(M_OPERATOR);
			member.addModes(M_VOICE);
		} else if (member.getUsername() == "Kees") {
			member.addModes(M_VOICE | M_INVISIBLE);
		}
		member.printModes();
	}

	for (auto &member : channel.getMembers()) {
		if (member.hasModes(M_OPERATOR)) {
			std::cout << member.getUsername() << " is an operator!" << std::endl;
		}
	}

	for (auto &member : channel.getMembers()) {
		if (member.hasModes(M_VOICE)) {
			std::cout << member.getUsername() << " has voice!" << std::endl;
		}
	}

	server.bindSocket(argv[1]);
	server.setPassword(argv[2]);
	server.start();
	return 0;
}
