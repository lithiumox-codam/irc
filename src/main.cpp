#include <signal.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "../include/Channel.hpp"
#include "../include/Server.hpp"

Server server;

void signalHandler(int signum) {
	std::cerr << "Interrupt signal (" << signum << ") received" << std::endl;

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
	Channel channel = Channel("General");
	channel.addUser(mees);

	if (channel.hasUser(mees)) {
		std::cout << "Mees is in the channel" << std::endl;
	}

	server.bindSocket(argv[1]);
	server.setPassword(argv[2]);
	server.start();
	return 0;
}
