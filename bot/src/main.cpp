#include "../include/Bot.hpp"
#include "../include/Epoll.hpp"

#include <string>
#include <iostream>

#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <string.h>

using namespace std;

int			server;
EpollClass	myEpoll;
Bot			bot;

static void createSocket(void) {
	// Create a socket
	server = ::socket(AF_INET, SOCK_STREAM, 0);

	int		optval = 1;
	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	if (server == -1) {
		cerr << "Error: socket creation failed" << '\n';
		exit(EXIT_FAILURE);
	}

	// Set socket to non-blocking
	int			flags = fcntl(server, F_GETFL, 0);

	if (fcntl(server, F_SETFL, flags | O_NONBLOCK) == -1) {
		cerr << "Error: fcntl failed" << '\n';
		exit(EXIT_FAILURE);
	}
}

static void connectToServer(const string &hostname, const string &port) {
	cout << "Connecting to server " << hostname << ":" << port << "..." << '\n';

	// Get server IP
	struct hostent	*host = gethostbyname(hostname.c_str());

	// Set server address and port
	struct sockaddr_in	addr = {
		.sin_family = AF_INET,										// IPv4
		.sin_port = htons(stoi(port)),				// Port
		.sin_addr = {(in_addr_t) *host->h_addr_list[0]},	// Server IP
		.sin_zero = {0}											// Padding
	};

	// Connect to the server
	if (connect(server, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		if (errno == EINPROGRESS) {
			return;
		}

		cerr << "Error: connect failed" << '\n';
		cerr << "Error: " << strerror(errno) << '\n';
		exit(EXIT_FAILURE);
	}
}

void	handleEvent(epoll_event& event) {
	if (event.events & EPOLLIN) {
		// Read from the socket
		bot.readFromServer();
	}

	if (event.events & EPOLLOUT) {
		// Write to the socket
		bot.sendToServer();
	}

	if (event.events & EPOLLERR) {
		cerr << "Error: EPOLLERR: " << strerror(errno) << '\n';
		exit (EXIT_FAILURE);
	}

	if (event.events & EPOLLHUP) {
		cerr << "Server shut down: EPOLLHUP" << '\n';
		exit(EXIT_FAILURE);
	}

	if (event.events & EPOLLRDHUP) {
		cerr << "Server shut down: EPOLLRDHUP" << '\n';
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	cout << "Starting IRC bot..." << '\n';

	if (argc != 3) {
		cerr << "Error: Server hostname and/or port not set by env. Please provide them like:" << '\n';
		cerr << "./ircbot [hostname] [port]" << '\n';
		exit (EXIT_FAILURE);
	}

	// Get server hostname and port
	string		hostname = argv[1];
	string		port = argv[2];

	// Create server socket
	createSocket();

	// Connect to the server
	connectToServer(hostname, port);

	// Add the server socket to the epoll
	myEpoll.add(server);

	// Join the server
	bot.join();

	// Listen for messages
	while (true) {
		myEpoll.wait();

		for (epoll_event &event : myEpoll.events) {
			if (event.events) {
				handleEvent(event);
			}
		}
	}

	// Get reply from chatGPT

	// Close the server socket
	close(server);

	return 0;
}