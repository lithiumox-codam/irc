#include <signal.h>

#include <cstdlib>
#include <iostream>

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

	server.bindSocket(argv[1]);
	server.setPassword(argv[2]);
	server.start();
	return 0;
}
