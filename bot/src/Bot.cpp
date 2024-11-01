#include "../include/Bot.hpp"
#include "../include/Epoll.hpp"

#include <sys/socket.h>
#include <sys/epoll.h>

extern EpollClass	myEpoll;
extern int			server;

// Constructors

Bot::Bot() {}

Bot &Bot::operator=(const Bot &other) {
	if (this != &other) {
		this->in_buffer = other.in_buffer;
		this->out_buffer = other.out_buffer;
	}
	return *this;
}

Bot::Bot(const Bot &other) {
	*this = other;
}

Bot::~Bot() {}

// Server Functions
void Bot::readFromServer(void) {
	char	buffer[4096];
	int		ret;

	ret = recv(server, buffer, sizeof(buffer) + 1, 0);

	if (ret == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return;
		}
		cerr << "Error: recv failed" << '\n';
		exit(EXIT_FAILURE);
	}

	if (ret == 0) {
		cerr << "Server connection lost..." << '\n';
		exit(EXIT_FAILURE);
	}

	buffer[ret] = '\0';
	this->in_buffer.append(buffer, ret);
	cout << "DEBUG: Received: " << this->in_buffer << '\n';
}

void Bot::sendToServer(void) {
	cout << "DEBUG: Sending: " << this->out_buffer << '\n';

	// Send the buffer
	while (!this->out_buffer.empty()) {
		int ret = send(server, this->out_buffer.data(), this->out_buffer.size(), 0);

		if (ret == -1) {
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
				return;
			}
			cerr << "Error: send failed" << '\n';
			exit(EXIT_FAILURE);
		}
		this->out_buffer.erase(0, ret);
	}

	// Change the epoll to remove the EPOLLOUT flag
	myEpoll.change(server, EPOLLIN);
}

void	Bot::addToBuffer(const string &data) {
	this->out_buffer.append(data);
}

void Bot::join(void) {
	cout << "Joining the server..." << '\n';

	// Join the server
	this->addToBuffer("NICK ircbot\r\n");
	this->addToBuffer("USER ircbot 0 * :ircbot\r\n");
	this->addToBuffer("CAP LS\r\n");
	this->addToBuffer("PASS test\r\n");
	this->addToBuffer("CAP END\r\n");

	// Join the #bot channel
	this->addToBuffer("JOIN #bot\r\n");

	// Set the epoll to be ready to write
	myEpoll.change(server, EPOLLIN | EPOLLOUT);
}

// Bot Functions

string Bot::getResponse(const string &query) {
	(void) query;
	return "Hello!";
}