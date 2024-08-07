#include <csignal>
#include <cstdlib>
#include <iostream>

#include "Channel.hpp"
#include "Server.hpp"
#include "User.hpp"

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

auto main(int argc, char **argv) -> int {
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	if (argc != 3) {
		cerr << "Usage: ./ircserver [port] [password]" << '\n';
		return 1;
	}

	cout << endl << endl;
	server.bindSocket(argv[1]);
	server.setPassword(argv[2]);
	server.start();
	return 0;
}
