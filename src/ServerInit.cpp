#include <sys/epoll.h>
#include <sys/socket.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "Exceptions.hpp"
#include "General.hpp"
#include "Server.hpp"

using namespace std;

#define MIN_PORT 0
#define MAX_PORT 65535

#define HOSTNAME "HOSTNAME"
#define PORT "PORT"
#define PASSWORD "PASSWORD"
#define OPERATORS "OPERATORS"
#define ENV_FILE ".env"

extern Server server;
map<string, string> env;

void Server::socketCreate() {
	this->socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (this->socket == -1) {
		throw SetUpException("Server socket creation failed");
	}

	int flags = fcntl(this->socket, F_GETFL, 0);

	if (flags == -1) {
		throw SetUpException("Getting server socket flags failed");
	}

	if (fcntl(this->socket, F_SETFL, flags | O_NONBLOCK) == -1) {
		throw SetUpException("Setting server socket flags failed");
	}

	int optval = 1;	 // true
	if (setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		throw SetUpException("Setting server socket options failed");
	}

	const struct sockaddr_in addr = {
		.sin_family = AF_INET, .sin_port = htons(this->port), .sin_addr = {INADDR_ANY}, .sin_zero = {0}};

	if (bind(this->socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		throw SetUpException("Binding server socket to port failed");
	}

	this->epollAdd(this->socket);
}

void Server::epollCreate() {
	this->epoll_fd = epoll_create1(0);
	if (this->epoll_fd == -1) {
		throw SetUpException("Creating epoll instance failed");
	}
}

void Server::setPort(string &port) {
	if (port.empty()) {
		throw ArgumentNotProvidedException(PORT);
	}

	int portNum;

	try {
		portNum = stoi(port);
	} catch (const std::out_of_range &e) {
		throw SetUpException("Port number out of stoi range: " + port);
	} catch (const std::exception &e) {
		throw SetUpException("Invalid port number: " + port);
	}

	if (portNum <= MIN_PORT || portNum > MAX_PORT) {
		throw SetUpException("Invalid port number: " + port);
	}

	this->port = portNum;
}

void Server::setPassword(string &password) {
	if (password.empty()) {
		throw ArgumentNotProvidedException(PASSWORD);
	}

	this->password = password;
}

void Server::setHostname(string &hostname) {
	if (hostname.empty()) {
		cerr << "Warning: HOSTNAME not set. Using default [localhost]" << '\n';
		hostname = "localhost";
	}

	this->hostname = hostname;
}

static void setOperators(string &operators) {
	if (operators.empty()) {
		cerr << "Warning: No server operators set" << '\n';
		return;
	}

	vector<string> opers = split(operators, ',');

	for (const string &oper : opers) {
		if (oper.empty()) {
			cerr << "Warning: Invalid operator" << '\n';
			continue;
		}

		if (server.operatorCheck(oper)) {
			cerr << "Warning: Operator already exists" << '\n';
			continue;
		}

		server.addOperator(oper);
	}
}

static void parseEnvFile(const string &filename) {
	std::ifstream file(filename);
	string line;

	if (!file.is_open()) {
		cerr << "Warning: Unable to find or open env file " << filename << '\n';
		return;
	}

	while (std::getline(file, line)) {
		if (file.bad()) {
			cerr << "Error: Unable to read env file " << filename << '\n';
			break;
		}

		auto [key, value] = splitPair(line, '=');

		env[key] = value;
	}

	file.close();
}


static void setFromSystemEnv(const string &key) {
	const char *value = getenv(key.c_str());

	env[key] = (value == nullptr) ? "" : value;
}


void Server::init(int argc, char **argv) {
	// First check the system environment
	setFromSystemEnv(PORT);
	setFromSystemEnv(PASSWORD);
	setFromSystemEnv(HOSTNAME);
	setFromSystemEnv(OPERATORS);

	// Then check the .env file and overwrite any values
	parseEnvFile(ENV_FILE);

	// Then check the command line arguments and overwrite any values
	if (argc > 1) {
		env[PORT] = argv[1];
	}

	if (argc > 2) {
		env[PASSWORD] = argv[2];
	}

	if (argc > 3) {
		env[HOSTNAME] = argv[3];
	}

	// Finally set the values inside the server
	setPort(env[PORT]);
	setPassword(env[PASSWORD]);
	setHostname(env[HOSTNAME]);
	setOperators(env[OPERATORS]);

	epollCreate();
	socketCreate();
}
