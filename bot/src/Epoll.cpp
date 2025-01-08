#include "../include/Epoll.hpp"
#include "../include/Bot.hpp"

#include <sys/epoll.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

using namespace std;

EpollClass::EpollClass() {
	this->fd = epoll_create1(0);
	if (this->fd == -1) {
		throw SetUpException("Creating an epoll instance failed");
	}
}

EpollClass &EpollClass::operator=(const EpollClass &other) {
	if (this != &other) {
		this->fd = other.fd;
	}
	return *this;
}

EpollClass::EpollClass(const EpollClass &other) {
	*this = other;
}

EpollClass::~EpollClass() {
	close(this->fd);
}

// Server functions

void EpollClass::add(int socket_fd) const {
	struct epoll_event event = {.events = EPOLLIN, .data = {.fd = socket_fd}};

	if (epoll_ctl(this->fd, EPOLL_CTL_ADD, socket_fd, &event) == -1) {
		throw ExecutionException("Adding a socket to the epoll instance failed");
	}
}

void EpollClass::change(int socket_fd, uint32_t events) {
	struct epoll_event event = {.events = events, .data = {.fd = socket_fd}};

	if (epoll_ctl(this->fd, EPOLL_CTL_MOD, socket_fd, &event) == -1) {
		throw ExecutionException("Changing epoll event data failed");
	}
}

void	EpollClass::wait() {
	this->numberOfEvents = epoll_wait(this->fd, this->events.data(), (int) MAXEVENTS, -1);

	if (numberOfEvents == -1) {
		cerr << strerror(errno) << '\n';
		cerr << "Error: epoll_wait failed" << '\n';
		exit(EXIT_FAILURE);
		throw ExecutionException("Waiting for epoll events failed");
	}
}