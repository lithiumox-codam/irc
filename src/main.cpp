#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Exceptions.hpp"
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
	signal(SIGKILL, signalHandler);

	getEnv();
	try {
		if (!server.isBound()) {
			if (argc < 2) {
				throw ArgumentNotProvidedException("Port");
			}
			server.bindSocket(argv[1]);
		}

		if (server.getPassword().empty()) {
			if (argc < 3) {
				throw ArgumentNotProvidedException("Password");
			}
			server.setPassword(argv[2]);
		}

		if (server.getHostname().empty()) {
			server.setHostname("localhost");
		}

		server.start();

	} catch (const ArgumentNotProvidedException &e) {
		cerr << "Error: " << e.what() << " not set by env. Please provide it like:" << '\n';
		cerr << "./ircserver [port] [password]" << '\n';
		return EXIT_FAILURE;

	} catch (const SetUpException &e) {
		cerr << "Error during setup: " << e.what() << ": " << strerror(errno) << '\n';
		return EXIT_FAILURE;

	} catch (const ExecutionException &e) {
		cerr << "Error during execution: " << e.what() << ": " << strerror(errno) << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
