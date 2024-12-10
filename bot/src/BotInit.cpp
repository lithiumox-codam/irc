#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <map>

#include "../include/Bot.hpp"
#include "../include/Epoll.hpp"

using namespace std;

#define MIN_PORT 0
#define MAX_PORT 65535

extern EpollClass myEpoll;
map<string, string> env;

void Bot::createSocket() {
	int socketfd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd == -1) {
		throw SetUpException("Server socket creation failed");
	}

	int flags = fcntl(socketfd, F_GETFL, 0);

	if (flags == -1) {
		throw SetUpException("Getting server socket flags failed");
	}

	if (fcntl(socketfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		throw SetUpException("Setting server socket flags failed");
	}

	int optval = 1; //true
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		throw SetUpException("Setting server socket options failed");
	}

	this->socketfd = socketfd;

	// Add the bot socket to the epoll interest list
	myEpoll.add(this->socketfd);
}

void waitForConnection(int socketfd) {
	int &error = errno;
	socklen_t len = sizeof(error);
	if (getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &error, &len) == -1) {
		throw SetUpException("Getting server socket options failed");
	}

	if (error != 0) {
		throw SetUpException("Connecting to server failed");
	}
}


void Bot::connectToServer(void) {
	struct addrinfo		preset;
	struct addrinfo		*res;

	memset(&preset, 0, sizeof(preset));
	preset.ai_family = AF_INET;			// IPv4
	preset.ai_socktype = SOCK_STREAM;	// TCP

	int status = getaddrinfo(env["HOSTNAME"].c_str(), env["PORT"].c_str(), &preset, &res);
	if (status != 0) {
		throw SetUpException("Getting server address info failed: " + string(gai_strerror(status)));
	}

	cout << "Connecting to " << env["HOSTNAME"] << ':' << env["PORT"] << "..." << '\n';

	if (connect(this->socketfd, res->ai_addr, res->ai_addrlen) == -1) {
		freeaddrinfo(res);

		if (errno == EINPROGRESS) {
			waitForConnection(this->socketfd);
			return;
		}

		throw SetUpException("Connecting to server failed");
	}

	freeaddrinfo(res);
}

static string getFromSystemEnv(const string &key) {
	const char *value = getenv(key.c_str());

	return (value == nullptr) ? "" : value;
}

static void setPort(void) {
	string &port = env["PORT"];

	if (port.empty()) {
		port = getFromSystemEnv("PORT");

		if (port.empty()) {
			cerr << "Warning: PORT not set. Using default [6667]" << '\n';
			port = "6667";
		}
	}
}

static void setPassword(void) {
	string &password = env["PASSWORD"];

	if (password.empty()) {
		password = getFromSystemEnv("PASSWORD");

		if (password.empty()) {
			cerr << "Warning: PASSWORD not set. Using default [test]" << '\n';
			password = "test";
		}
	}
}

static void setHostname(void) {
	string &hostname = env["HOSTNAME"];

	if (hostname.empty()) {
		hostname = getFromSystemEnv("HOSTNAME");

		if (hostname.empty()) {
			cerr << "Warning: HOSTNAME not set. Using default [localhost]" << '\n';
			hostname = "localhost";
		}
	}
}

static pair<string, string> splitPair(const string &str, const char &delim) {
	size_t found = str.find(delim);
	if (found == string::npos) {
		return make_pair(str, "");
	}
	string first = str.substr(0, found);
	string second = str.substr(found + 1);

	if (!second.empty() && second[0] == ':') {
		second = second.substr(1);
	}
	return make_pair(first, second);
}

static void parseEnvFile(const string &filename) {
	std::ifstream file(filename);
	string line;

	if (!file.is_open()) {
		cerr << "Error: Unable to open env file " << filename << '\n';
		return ;
	}

	while (std::getline(file, line)) {
		if (file.bad()) {
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

void Bot::init(int argc, char **argv) {
	if (argc > 1) {
		env["HOSTNAME"] = argv[1];
	}

	if (argc > 2) {
		env["PORT"] = argv[2];
	}

	if (argc > 3) {
		env["PASSWORD"] = argv[3];
	}

	if (argc > 4) {
		parseEnvFile(argv[4]);
	}

	// Set the hostname, port and password in the environment
	setHostname();
	setPort();
	setPassword();

	// Create the bot socket
	this->createSocket();

	// Connect to the server
	this->connectToServer();

	// Join the server
	this->join(env["PASSWORD"]);
}
