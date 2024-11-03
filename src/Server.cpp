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
#include <stdexcept>
#include <string>
#include <vector>

#include "General.hpp"
#include "User.hpp"
#include "Exceptions.hpp"

extern Server server;

Server::Server() : socket(0), port(0), running(false) { }

Server::~Server() { this->stop(); }

void Server::setPassword(const string &password) { this->password = password; }

const string &Server::getPassword() const { return this->password; }

void Server::bindSocket(const string &portString) {
	in_port_t port = htons(stoi(portString));

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

void Server::setHostname(const string &hostString) { hostname = hostString; }

void Server::userReadyToSend(User &user) {
	this->myEpoll.change(user.getSocket(), EPOLLIN | EPOLLOUT);
}

void Server::acceptNewConnection() {
	const int clientSocket = accept(this->socket, nullptr, nullptr);

	if (clientSocket == -1) {
		if (errno == EWOULDBLOCK) {
			return;
		}
		cerr << strerror(errno) << '\n';
		cerr << "Error: accept failed" << '\n';
		exit(EXIT_FAILURE);
	}

	bool opt = true;
	setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	this->addUser(clientSocket);

	cout << GREEN << "New connection on socket " << clientSocket << RESET << '\n';
}

void	Server::handleEvent(epoll_event& event) {
	if (event.data.fd == this->socket && event.events & EPOLLIN) {
		acceptNewConnection();
		return ;
	}

	try {
		User *user = server.getUser(event.data.fd);

		if (event.events & EPOLLIN) {
			if (!user->readFromSocket()) { 
				server.removeUser(*user);
				return ;
			}
		}

		if (event.events & EPOLLOUT) {
			if (!user->sendToSocket()) {
				server.removeUser(*user);
				return ;
			}

			if (user->getOutBuffer().empty()) {
				this->myEpoll.change(event.data.fd, EPOLLIN);
			}
		}

		if (event.events & EPOLLERR) {
			cerr << "Error: EPOLLERR: " << strerror(errno) << '\n';
			server.removeUser(*user);
			return ;
		}

		if (event.events & EPOLLHUP) {
			cerr << "Client shut down: EPOLLHUP" << '\n';
			server.removeUser(*user);
			return ;
		}

		if (event.events & EPOLLRDHUP) {
			cerr << "client shut down: EPOLLRDHUP" << '\n';
			server.removeUser(*user);
			return ;
		}
	} catch (const runtime_error &e) {
		cerr << "Error: " << e.what() << '\n';
	}
}

void Server::start() {
	if (listen(this->socket, (int)ServerConfig::BACKLOG) != -1) {
		cout << "Server started on socket fd " << socket << '\n';
		cout << "Press Ctrl+C to stop the server" << '\n';
		cout << "Password: " << this->password << '\n';
		cout << "\nListening for incoming connections..." << '\n';
	} else {
		cerr << "Error: listen failed" << '\n';
		return;
	}
	this->running = true;

	this->myEpoll.add(this->socket);

	string message;

	while (true) {
		this->myEpoll.wait();

		for (epoll_event &event : this->myEpoll.events) {
			if (event.events) {
				handleEvent(event);
			}
		}
	}
}

void Server::stop() {
	if (!this->running) {
		return;
	}
	close(this->socket);
	this->running = false;

	cout << RED << "\rServer stopped" << RESET << '\n';
}

deque<User> &Server::getUsers() { return this->users; }

User *Server::getUser(const int socket) {
	for (auto &user : this->users) {
		if (user.getSocket() == socket) {
			return &user;
		}
	}
	throw runtime_error("User not found");
}

User *Server::getUser(const string &nickname) {
	for (auto &user : this->users) {
		cout << "User: " << user.getNickname() << '\n';
		if (user.getNickname() == nickname) {
			return &user;
		}
	}
	throw runtime_error("User not found");
}

void Server::addUser(unsigned int socket) {
	this->users.emplace_back(socket);
	this->myEpoll.add(socket);
}

void Server::removeUser(User &user) {
	//remove from channels
	for (auto &channel : this->channels) {
		if (channel.hasUser(&user)) {
			channel.removeUser(&user);

			if (channel.getMembers()->empty()) {
				this->removeChannel(channel);
			}
		}
	}

	for (auto it = this->users.begin(); it != this->users.end(); ++it) {
		if (it->getSocket() == user.getSocket()) {
			this->myEpoll.remove(user.getSocket());
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
	throw runtime_error("Channel " + name + " not found");
}

const string &Server::getHostname() { return this->hostname; }

bool Server::isBound() const { return this->socket != 0; }

void Server::addOperator(const string &nickname) { this->operators.push_back(nickname); }

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
