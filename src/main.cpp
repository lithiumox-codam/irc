#include <csignal>
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
	getEnv();
	if (server.getPassword().empty() && !server.isBound()) {
		if (argc != 3) {
			cerr << "Error: Password and/or port not set by env. Please provide them like:" << '\n';
			cerr << "./ircserver [port] [password]" << '\n';
			return 1;
		}
		if (server.getHostname().empty()) {
			server.setHostname("localhost");
		}
		server.bindSocket(argv[1]);
		server.setPassword(argv[2]);
	} else {
		if (server.getPassword().empty() || !server.isBound()) {
			cerr << "Error: Password and/or port not set by env. Please provide them like:" << '\n';
			cerr << "./ircserver [port] [password]" << '\n';
			return 1;
		}
	}

	server.start();
	return 0;
}
