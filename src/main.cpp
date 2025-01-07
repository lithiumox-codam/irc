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
			exit(EXIT_SUCCESS);
			break;
		case SIGTERM:  // kill command
			server.stop();
			exit(signum);
			break;
	}
}

int main(int argc, char **argv) {
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGKILL, signalHandler);

	try {
		server.init(argc, argv);
		server.start();

	} catch (const ArgumentNotProvidedException &e) {
		cerr << "Error: " << e.what() << " not set by environment or parameter." << '\n';
		cerr << "Please provide it like:\n\n";
		cerr << "       ./ircserver [port] [password]" << '\n';
		cerr << "                 or" << '\n';
		cerr << "PORT=[port] PASSWORD=[password] ./ircserver" << '\n';
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
