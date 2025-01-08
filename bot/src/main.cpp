#include "../include/Bot.hpp"
#include "../include/Epoll.hpp"

#include <csignal>
#include <cstdlib>
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

static inline string epollErrorEvent(int socketfd) {
	int err;
	socklen_t len = sizeof(err);

	getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &err, &len);

	return strerror(err);
}

static void	handleEvent(epoll_event& event) {
	bool	disconnect = false;
	string	reason;

	if ((event.events & EPOLLIN) != 0) {
		try {
			bot.readFromServer();
		} catch (const ExecutionException &e) {
			disconnect = true;
			reason = e.what();
		}
	}

	if ((event.events & EPOLLOUT) != 0) {
		try {
			bot.sendToServer();
		} catch (const ExecutionException &e) {
			disconnect = true;
			reason = e.what();
		}
	}

	if ((event.events & EPOLLHUP) != 0) {
		disconnect = true;
		reason = "Server shut down (HANGUP)";
	}

	if ((event.events & EPOLLRDHUP) != 0) {
		disconnect = true;
		reason = "Server shut down (READ HANGUP)";
	}

	if ((event.events & EPOLLERR) != 0) {
		disconnect = true;
		reason = "Error on socket: " + epollErrorEvent(event.data.fd);
	}

	if (disconnect) {
		throw ExecutionException(reason);
	}
}

static void start(void) {
	while (true) {
		myEpoll.wait();

		for (int i = 0; i < myEpoll.numberOfEvents; i++) {
			handleEvent(myEpoll.events[i]);
		}
	}
}

void signalHandler(int signum) {
	switch (signum) {
		case SIGINT:  // ctrl + C
			exit(EXIT_SUCCESS);
			break;
		case SIGTERM:  // kill command
			exit(signum);
			break;
	}
}

int main(int argc, char **argv) {
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	try {
		bot.init(argc, argv);
		start();
	}
	catch (const SetUpException &e) {
		cerr << "Error during setup: " << e.what() << ": " << strerror(errno) << '\n';
		return EXIT_FAILURE;
	} catch (const ExecutionException &e) {
		cerr << "Error during execution: " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}