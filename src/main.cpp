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

	User mees = User("Mees", "Joe Mama");
	User kees = User("Kees", "Joe Mama");
	Channel channel = Channel("General");
	channel.addUser(mees);
	channel.addUser(kees);

	for (auto &member : channel.getMembers()) {
		if (member.getUsername() == "Mees") {
			member.setPermissions(P_ALL);
		} else if (member.getUsername() == "Kees") {
			member.setPermissions(P_READ | P_WRITE);
			member.addPermissions(P_BAN);
		}
		member.printPermissions();
	}

	// list all the members of the channel with ban permissions
	for (auto &member : channel.getMembers()) {
		if (member.hasPermissions(P_KICK | P_MANAGE)) {
			std::cout << member.getUsername() << " has kick permissions" << std::endl;
		}
	}

	server.bindSocket(argv[1]);
	server.setPassword(argv[2]);
	server.start();
	return 0;
}
