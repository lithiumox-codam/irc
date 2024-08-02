#include <signal.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "../include/Channel.hpp"
#include "../include/Server.hpp"

Server server;

void signalHandler(int signum) {
	std::cerr << "Interrupt signal (" << signum << ") received" << std::endl;
	switch (signum) {
		case SIGINT:  // ctrl + c
			std::cerr << "Exiting..." << std::endl;
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
			member.setPermissions(PERMISSIONS_READ | PERMISSIONS_WRITE);
		}
		member.printPermissions();
	}

	server.bindSocket(argv[1]);
	server.setPassword(argv[2]);
	server.start();
	return 0;
}
