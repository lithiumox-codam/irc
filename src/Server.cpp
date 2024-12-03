#include "Server.hpp"

#include <limits.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Exceptions.hpp"
#include "IRStream.hpp"
#include "User.hpp"

extern Server server;

Server::Server() : socket(0), port(0), running(false) {}

Server::~Server() { this->stop(); }

void Server::setPassword(const string &password) { this->password = password; }

const string &Server::getPassword() const { return this->password; }

void Server::bindSocket(const string &portString) {
	in_port_t port = htons(stoi(portString));

	this->socket = ::socket(AF_INET, SOCK_STREAM, 0);

	if (this->socket == -1) {
		throw SetUpException("Socket creation failed");
	}

	if (fcntl(this->socket, F_SETFL, O_NONBLOCK) == -1) {
		throw ExecutionException("Setting server socket flags failed");
	}

	int flags = fcntl(this->socket, F_GETFL, 0);

	if (flags == -1) {
		throw SetUpException("Getting socket flags failed");
	}

	if (fcntl(this->socket, F_SETFL, flags | O_NONBLOCK) == -1) {
		throw SetUpException("Setting socket flags failed");
	}

	const struct sockaddr_in addr = {
		.sin_family = AF_INET, .sin_port = port, .sin_addr = {INADDR_ANY}, .sin_zero = {0}};

	if (bind(this->socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		throw SetUpException("Socket bind failed");
	}

	cout << "Socket bound to port " << portString << '\n';
}

void Server::setHostname(const string &hostString) { hostname = hostString; }

void Server::epollCreate() {
	this->epoll_fd = epoll_create1(0);
	if (this->epoll_fd == -1) {
		throw SetUpException("Creating epoll instance failed");
	}
}

void Server::epollAdd(int socket_fd) const {
	struct epoll_event event = {.events = EPOLLIN, .data = {.fd = socket_fd}};

	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1) {
		throw ExecutionException("Adding a socket to the epoll instance failed");
	}
}

void Server::epollRemove(int socket_fd) const {
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, socket_fd, nullptr) == -1) {
		throw ExecutionException("Removing a socket from the epoll instance failed");
	}
}

void Server::epollChange(int socket_fd, uint32_t events) const {
	struct epoll_event event = {.events = events, .data = {.fd = socket_fd}};

	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, socket_fd, &event) == -1) {
		throw ExecutionException("Changing epoll event data failed");
	}
}

static string epollErrorEvent(User *user) {
	int err;

	socklen_t len = sizeof(err);
	getsockopt(user->getSocket(), SOL_SOCKET, SO_ERROR, &err, &len);
	return strerror(err);
}

static void epollEvent(struct epoll_event &event) {
	int socket_fd = event.data.fd;
	User *user = server.getUser(socket_fd);
	bool removeUser = false;
	string reason;

	if ((event.events & EPOLLIN) != 0) {
		try {
			user->readFromSocket();
			user->parse();
		} catch (UserQuitException &e) {
			removeUser = true;
			reason = e.what();
		}
	}

	if ((event.events & EPOLLOUT) != 0) {
		try {
			user->sendToSocket();
		} catch (UserQuitException &e) {
			removeUser = true;
			reason = e.what();
		}
	}

	if ((event.events & EPOLLERR) != 0) {
		removeUser = true;
		reason = "Error on socket: " + epollErrorEvent(user);
	}

	if ((event.events & EPOLLHUP) != 0) {
		removeUser = true;
		reason = "Client shutdown (HANGUP)";
	}

	if ((event.events & EPOLLRDHUP) != 0) {
		removeUser = true;
		reason = "Client shutdown (READ HANGUP)";
	}

	if (removeUser) {
		server.removeUser(*user, reason);
	}
}

void Server::acceptNewConnection() {
	const int clientSocket = accept(this->socket, nullptr, nullptr);

	if (clientSocket == -1) {
		if (errno == EWOULDBLOCK) {
			return;
		}
		throw ExecutionException("Accepting new connection failed");
	}

	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
		throw ExecutionException("Setting client socket flags failed");
	}

	this->addUser(clientSocket);
}

void Server::handleEvents(array<struct epoll_event, (size_t)ServerConfig::BACKLOG> &events, int numberOfEvents) {
	for (int i = 0; i < numberOfEvents; i++) {
		struct epoll_event &event = events[i];

		if (event.data.fd == socket && (event.events & EPOLLIN)) {
			acceptNewConnection();
		} else {
			epollEvent(event);
		}
	}
}

void Server::start() {
	if (listen(this->socket, (int)ServerConfig::BACKLOG) == -1) {
		throw SetUpException("Listening on socket failed");
	}

	this->running = true;
	cout << "Server started on socket fd " << socket << '\n';
	cout << "Press Ctrl+C to stop the server" << '\n';
	cout << "Password: " << this->password << '\n';
	cout << "\nListening for incoming connections..." << '\n';

	this->epollCreate();

	string message;
	this->epollAdd(this->socket);
	while (true) {
		const int maxEvents = 10;
		array<struct epoll_event, maxEvents> events;
		int numberOfEvents = epoll_wait(this->epoll_fd, events.data(), (int)ServerConfig::MAX_EVENTS, -1);

		if (numberOfEvents == -1) {
			throw ExecutionException("Waiting for epoll events failed");
		}

		handleEvents(events, numberOfEvents);
	}
}

void Server::stop() {
	if (!this->running) {
		return;
	}
	close(this->socket);
	this->running = false;
}

deque<User> &Server::getUsers() { return this->users; }

User *Server::getUser(const int socket) {
	for (auto &user : this->users) {
		if (user.getSocket() == socket) {
			return &user;
		}
	}
	throw NoSuchUserException("");	////////////////////change this later maybe
}

User *Server::getUser(const string &nickname) {
	for (auto &user : this->users) {
		cout << "User: " << user.getNickname() << '\n';
		if (user.getNickname() == nickname) {
			return &user;
		}
	}
	throw NoSuchUserException(nickname);
}

void Server::addUser(unsigned int socket) {
	this->users.emplace_back(socket);
	this->epollAdd(socket);
}

void Server::removeUser(User &user, const string &reason) {
	IRStream stream;

	stream.prefix(&user).param("QUIT").trail(reason).end();
	// remove from channels
	for (auto &channel : this->channels) {
		if (channel.hasUser(&user)) {
			channel.broadcast(stream, &user);
			channel.removeUser(&user);

			if (channel.getMembers()->empty()) {
				this->removeChannel(channel);
			}
		}
	}

	for (auto it = this->users.begin(); it != this->users.end(); ++it) {
		if (it->getSocket() == user.getSocket()) {
			this->epollRemove(user.getSocket());
			user.closeSocket();
			this->users.erase(it);
			break;
		}
	}
}

Channel &Server::addChannel(const string &channelName) { return this->channels.emplace_back(channelName); }

void Server::removeChannel(Channel &channel) {
	for (auto it = this->channels.begin(); it != this->channels.end(); ++it) {
		if (it->getName() == channel.getName()) {
			this->channels.erase(it);
			break;
		}
	}
}

deque<Channel> &Server::getChannels() { return this->channels; }

Channel *Server::getChannel(const string &name) {
	for (auto &channel : this->channels) {
		if (channel.getName() == name) {
			return &channel;
		}
	}
	throw NoSuchChannelException(name);
}

const string &Server::getHostname() { return this->hostname; }

bool Server::isBound() const { return this->socket != 0; }

void Server::addOperator(const string &nickname) { this->operators.push_back(nickname); }

void Server::removeOperator(const string &nickname) {
	for (auto it = this->operators.begin(); it != this->operators.end(); ++it) {
		if (*it == nickname) {
			this->operators.erase(it);
			break;
		}
	}
}

bool Server::operatorCheck(User *user) const {
	if (this->operators.empty()) {
		return false;
	}
	// NOLINTNEXTLINE
	for (const string &oper : this->operators) {
		if (oper == user->getNickname()) {
			return true;
		}
	}
	return false;
}

string Server::getUserCount() const {
	size_t count = 0;
	for (const auto &user : users) {
		if (user.hasHandshake(H_AUTHENTICATED)) {
			count++;
		}
	}
	return to_string(count);
}
