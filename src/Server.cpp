#include "Server.hpp"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "User.hpp"

extern Server server;

Server::Server() : socket(0), port(0), running(false) {
	string hostname((int)ServerConfig::BUFFER_SIZE, '\0');

	try {
		gethostname(hostname.data(), hostname.size());
		this->hostname = hostname;
	} catch (const exception &e) {
		cerr << "Error: " << e.what() << '\n';
	}
}

Server::~Server() { this->stop(); }

void Server::setPassword(const string &password) { this->password = password; }

auto Server::getPassword() const -> const string & { return this->password; }

void Server::bindSocket(const string &portString) {
	// Set the port
	in_port_t port = htons(stoi(portString));

	// Create a socket
	this->socket = ::socket(AF_INET, SOCK_STREAM, 0);
	int reuseAddr = 1;
	setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));

	if (this->socket == -1) {
		cerr << "Error: socket creation failed" << '\n';
		exit(EXIT_FAILURE);
	}

	int flags = fcntl(this->socket, F_GETFL, 0);

	if (fcntl(this->socket, F_SETFL, flags | O_NONBLOCK) == -1) {
		cerr << "Error: fcntl failed" << '\n';
		exit(EXIT_FAILURE);
	}

	const struct sockaddr_in addr = {
		.sin_family = AF_INET, .sin_port = port, .sin_addr = {INADDR_ANY}, .sin_zero = {0}};

	if (bind(this->socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		cerr << "Error: bind failed" << '\n';
		exit(EXIT_FAILURE);
	}
	cout << "Socket bound to port " << portString << '\n';
}

static auto createEpoll() -> int {
	int epollFd = epoll_create1(0);
	if (epollFd == -1) {
		cerr << strerror(errno) << '\n';
		cerr << "Error: epoll_create1 failed" << '\n';
		exit(EXIT_FAILURE);
	}
	return epollFd;
}

static void addToEpoll(int epollFD, int socketFD) {
	struct epoll_event event = {.events = EPOLLIN, .data = {.fd = socketFD}};

	cout << "Adding user with socket: " << socketFD << " to epoll" << '\n';

	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, socketFD, &event) == -1) {
		cerr << strerror(errno) << '\n';
		cerr << "Error: epoll_ctl failed" << '\n';
		exit(EXIT_FAILURE);
	}
}

static void handleEvent(int epollFD, struct epoll_event &event) {
	int		socketFD = event.data.fd;
	User	&user = server.getUser(socketFD);

	if (event.events & EPOLLERR) {	// Error on the socket
		cerr << "Error: EPOLLERR" << '\n';

		socklen_t len = sizeof(errno);
		getsockopt(socketFD, SOL_SOCKET, SO_ERROR, &errno, &len);
		cerr << strerror(errno) << '\n';

		epoll_ctl(epollFD, EPOLL_CTL_DEL, socketFD, nullptr);
		server.removeUser(user);
	}
	if (event.events & EPOLLHUP) {
		cerr << "Error: EPOLLHUP" << '\n';
		epoll_ctl(epollFD, EPOLL_CTL_DEL, socketFD, nullptr);
		server.removeUser(user);
	}
	if (event.events & EPOLLRDHUP) {
		cerr << "Error: EPOLLRDHUP" << '\n';
		epoll_ctl(epollFD, EPOLL_CTL_DEL, socketFD, nullptr);
		server.removeUser(user);
	}

	if (event.events & EPOLLIN) {
		int ret = user.readFromSocket();

		if (ret > 0) {
			cout << "Received message from " << user << '\n';
		}
		if (ret == 0) {
			cout << "User " << user << " gracefully disconnected" << '\n';
			epoll_ctl(epollFD, EPOLL_CTL_DEL, socketFD, nullptr);
			server.removeUser(user);
		}
		if (ret == -1) {
			cerr << "Error: recv failed" << '\n';

			if (errno == EWOULDBLOCK || errno == EAGAIN) {
				return;
			}

			epoll_ctl(epollFD, EPOLL_CTL_DEL, socketFD, nullptr);
			server.removeUser(user);
		}
	}
}

static void pollUsers(int epollFD) {
	const int maxEvents = 10;
	array<struct epoll_event, maxEvents> events;
	int numberOfEvents = epoll_wait(epollFD, events.data(), (int)ServerConfig::MAX_EVENTS, 0);
	if (numberOfEvents == -1) {
		cerr << strerror(errno) << '\n';
		cerr << "Error: epoll_wait failed" << '\n';
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < numberOfEvents; i++) {
		handleEvent(epollFD, events[i]);
	}
}

void Server::start() {
	// Listen for incoming connections, with a backlog of 10 pending connections
	if (listen(this->socket, (int)ServerConfig::BACKLOG) != -1) {
		cout << "Server started on socket fd " << this->socket << '\n';
		cout << "Press Ctrl+C to stop the server" << '\n';
		cout << "Password: " << this->password << '\n';
		cout << "\nListening for incoming connections..." << '\n';
	} else {
		cerr << "Error: listen failed" << '\n';
		return;
	}
	this->running = true;

	int epollFD = createEpoll();

	string message;
	while (true) {
		pollUsers(epollFD);

		// Accept a connection
		const int clientSocket = accept(this->socket, nullptr, nullptr);

		if (clientSocket == -1) {
			if (errno == EWOULDBLOCK) {
				continue;
			}

			cerr << strerror(errno) << '\n';
			cerr << "Error: accept failed" << '\n';
			exit(EXIT_FAILURE);
		}

		bool opt = true;
		setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		this->addUser(clientSocket);
		addToEpoll(epollFD, clientSocket);
	}
}

void Server::stop() {
	if (!this->running) {
		return;
	}
	cout << "\rServer stopped" << '\n';
	close(this->socket);
	this->running = false;
}

auto Server::getUsers() const -> const vector<User> & { return this->users; }

auto Server::getUser(const int socket) -> User & {
	for (auto &user : this->users) {
		if (user.getSocket() == socket) {
			return user;
		}
	}
	throw runtime_error("User not found");
}

auto Server::addUser(unsigned int socket) -> User & { return this->users.emplace_back(socket); }

void Server::removeUser(User &user) {
	for (auto it = this->users.begin(); it != this->users.end(); ++it) {
		if (it->getSocket() == user.getSocket()) {
			user.closeSocket();
			this->users.erase(it);
			break;
		}
	}
}

auto Server::addChannel(string &channelName) -> Channel & { return this->channels.emplace_back(channelName); }

void Server::removeChannel(Channel &channel) {
	for (auto it = this->channels.begin(); it != this->channels.end(); ++it) {
		if (it->getName() == channel.getName()) {
			this->channels.erase(it);
			break;
		}
	}
}

auto Server::getChannels() -> vector<Channel> & { return this->channels; }

auto Server::getChannel(const string &name) -> Channel & {
	for (auto &channel : this->channels) {
		if (channel.getName() == name) {
			return channel;
		}
	}
	throw runtime_error("Channel not found");
}
