#include <cstdlib>
#include <iostream>

#include "../include/Server.hpp"

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

	Server server(argv[2], argv[1]);
	atexit(server.stop());

	return 0;
}
