#include "Server.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sys/epoll.h>

#include "General.hpp"
#include "User.hpp"

#define MAX_EVENTS 10

Server::Server() : password(""), socket(0), port(0), running(false) {
	char hostname[1024];
	try {
		gethostname(hostname, 1024);
		this->hostname = hostname;
	} catch (const exception &e) {
		cerr << "Error: " << e.what() << endl;
	}
}

Server::~Server() { this->stop(); }

void Server::setPassword(const string &password) { this->password = password; }

const string &Server::getPassword() const { return this->password; }

void Server::bindSocket(const string &portString) {
	// Set the port
	in_port_t port = htons(stoi(portString));

	// Create a socket
	this->socket = ::socket(AF_INET, SOCK_STREAM, 0);
	int reuseAddr = 1;
	setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));

	if (this->socket == -1) {
		cerr << "Error: socket creation failed" << endl;
		exit(EXIT_FAILURE);
	}

	int flags = fcntl(this->socket, F_GETFL, 0);

	if (fcntl(this->socket, F_SETFL, flags | O_NONBLOCK) == -1) {
		cerr << "Error: fcntl failed" << endl;
		exit(EXIT_FAILURE);
	}

	const struct sockaddr_in addr = {
		.sin_family = AF_INET, .sin_port = port, .sin_addr = {INADDR_ANY}, .sin_zero = {0}};

	if (bind(this->socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		cerr << "Error: bind failed" << endl;
		exit(EXIT_FAILURE);
	}
	cout << "Socket bound to port " << portString << endl;
}

static int createEpoll() {
	int epollFd = epoll_create1(0);
	if (epollFd == -1) {
		cerr << strerror(errno) << endl;
		cerr << "Error: epoll_create1 failed" << endl;
		exit(EXIT_FAILURE);
	}
	return epollFd;
}

static void addUserToEpoll(int epollFD, User *user)
{
	struct epoll_event event = { .events = EPOLLIN, .data = { .ptr = (void *) user } };

	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, user->getSocket(), &event) == -1) {
		cerr << strerror(errno) << endl;
		cerr << "Error: epoll_ctl failed" << endl;
		exit(EXIT_FAILURE);
	}
}

static string receiveMessage(int client) {
	string message;
	int bytesReceived;
	char buffer[BUFFER_SIZE];

	while (1) {
		bytesReceived = recv(client, buffer, BUFFER_SIZE, 0);

		if (bytesReceived == -1) {
			if (errno == EWOULDBLOCK) continue;

			cerr << strerror(errno) << endl;
			cerr << "Error: recv failed" << endl;
			exit(EXIT_FAILURE);
		} else if (bytesReceived == 0) {
			cout << "Connection closed" << endl;
			close(client);
			return nullptr;
		}

		cout << "Received " << bytesReceived << " bytes" << endl;
		buffer[bytesReceived] = '\0';
		return message.append(buffer);
	}
}

static void pollUsers(int epollFD) {
	struct epoll_event events[10];
	int numberOfEvents = epoll_wait(epollFD, events, MAX_EVENTS, 0);
	if (numberOfEvents == -1) {
		cerr << strerror(errno) << endl;
		cerr << "Error: epoll_wait failed" << endl;
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < numberOfEvents; i++) {
		User *user = (User *) events[i].data.ptr;

		cout << "Received message from user with socket file descriptor " << user->getSocket() << endl;

		string message = receiveMessage(user->getSocket());

		if (message.empty()) continue;
		if (message.ends_with("\r\n")) {
			unordered_map<PacketType, string> parsed = parse(message);

			for (auto &pair : parsed) {
				cout << pair.first << "\t" << pair.second << endl;
			}

			PacketProcessor(parsed, user->getSocket());
		}
	}
}

void Server::start(void) {
	// Listen for incoming connections, with a backlog of 10 pending connections
	if (listen(this->socket, 10) == -1) {
		cerr << "Error: listen failed" << endl;
		return;
	} else {
		cout << "Server started on socket fd " << this->socket << endl;
		cout << "Press Ctrl+C to stop the server" << endl;
		cout << "Password: " << this->password << endl;
		cout << "\nListening for incoming connections..." << endl;
	}
	this->running = true;

	int epollFD = createEpoll();

	string message;
	while (1) {
		pollUsers(epollFD);

		// Accept a connection
		const int clientSocket = accept(this->socket, NULL, NULL);

		if (clientSocket == -1) {
			if (errno == EWOULDBLOCK) continue;

			cerr << strerror(errno) << endl;
			cerr << "Error: accept failed" << endl;
			exit(EXIT_FAILURE);
		} else {
			User *newUser = new User(clientSocket);
			this->addUser(newUser);
			addUserToEpoll(epollFD, newUser);
			cout << "Connection accepted" << endl;
		}
	}
}

void Server::sendMessage(int client, const string &message) {
	if (send(client, message.c_str(), message.length(), 0) == -1) {
		cerr << strerror(errno) << endl;
		cerr << "Error: send failed" << endl;
		exit(EXIT_FAILURE);
	}
}

void Server::stop(void) {
	if (!this->running) return;
	cout << "\rServer stopped" << endl;
	close(this->socket);
	this->running = false;
}

vector<User *> Server::getUsers(void) { return this->users; }

void Server::addUser(User *user) { this->users.push_back(user); }

void Server::removeUser(User *user) {
	for (auto it = this->users.begin(); it != this->users.end(); ++it) {
		if ((*it)->getSocket() == user->getSocket()) {
			this->users.erase(it);
			break;
		}
	}
}
