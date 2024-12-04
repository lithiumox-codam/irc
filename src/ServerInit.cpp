#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <sys/epoll.h>
#include <sys/socket.h>

#include "Server.hpp"
#include "General.hpp"
#include "Exceptions.hpp"

using namespace std;

#define MIN_PORT 0
#define MAX_PORT 65535

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

	int optval = 1; //true
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

static string getFromSystemEnv(const string &key) {
	const char *value = getenv(key.c_str());

	if (value == nullptr) {
		return "";
	}

	return value;
}

void Server::setPort(string &port) {
	if (port.empty()) {
		port = getFromSystemEnv("PORT");

		if (port.empty()) {
			throw ArgumentNotProvidedException("PORT");
		}
	}

	int portNum = stoi(port);

	if (portNum <= MIN_PORT || portNum > MAX_PORT) {
		throw SetUpException("Invalid port number: " + port);
	}

	this->port = portNum;
}

void Server::setPassword(string &password) {
	if (password.empty()) {
		password = getFromSystemEnv("PASSWORD");

		if (password.empty()) {
			throw ArgumentNotProvidedException("PASSWORD");
		}
	}

	this->password = password;
}

void Server::setHostname(string &hostname) {
	if (hostname.empty()) {
		hostname = getFromSystemEnv("HOSTNAME");

		if (hostname.empty()) {
			cerr << "Error: HOSTNAME not set. Using default [localhost]" << '\n';
			hostname = "localhost";
		}
	}

	this->hostname = hostname;
}

static void setOperators(string &operators) {
	if (operators.empty()) {
		operators = getFromSystemEnv("OPERATORS");

		if (operators.empty()) {
			cerr << "Error: OPERATORS not set" << '\n';
			return;
		}
	}

	vector<string> opers = split(operators, ',');

	for (const string &oper : opers) {
		if (oper.empty()) {
			cerr << "Error: Invalid operator" << '\n';
			continue;
		}

		if (server.operatorCheck(oper)) {
			cerr << "Error: Operator already exists" << '\n';
			continue;
		}

		server.addOperator(oper);
	}
}

static void parseEnvFile(const string &filename) {
	std::ifstream file(filename);
	string line;

	if (!file.is_open()) {
		cerr << "Error: Unable to open env file " << filename << '\n';
		return ;
	}

	while (std::getline(file, line)) {
		if (file.fail()) {
			cerr << "Error: Unable to read env file " << filename << '\n';
			break ;
		}

		auto [key, value] = splitPair(line, '=');

		if (env[key].empty()) {
			env[key] = value;
		}
	}

	file.close();
}

void Server::init(int argc, char **argv) {
	if (argc > 1) {
		env["PORT"] = argv[1];
	}

	if (argc > 2) {
		env["PASSWORD"] = argv[2];
	}

	if (argc > 3) {
		parseEnvFile(argv[3]);
	}

	setPort(env["PORT"]);
	setPassword(env["PASSWORD"]);
	setHostname(env["HOSTNAME"]);
	setOperators(env["OPERATORS"]);

	epollCreate();
	socketCreate();
}
