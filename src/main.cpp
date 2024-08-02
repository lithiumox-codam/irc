#include <cstdlib>
#include <iostream>

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

	server.bindSocket(argv[1]);
	server.setPassword(argv[2]);
	server.start();
	return 0;
}
