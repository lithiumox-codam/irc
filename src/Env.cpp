#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Server.hpp"

#define MIN_PORT 0
#define MAX_PORT 65535

extern Server server;

static bool checkPort(const string &port) {
	if (port.empty() || stoi(port) <= MIN_PORT || stoi(port) > MAX_PORT) {
		cerr << "Error: Invalid port number" << '\n';
		return false;
	}
	return true;
}

static bool checkPassword(const string &password) {
	if (password.empty()) {
		cerr << "Error: Invalid password" << '\n';
		return false;
	}
	return true;
}

static bool checkHostname(const string &hostname) {
	if (hostname.empty()) {
		cerr << "Error: Invalid hostname" << '\n';
		return false;
	}
	return true;
}

static void checkEnv() {
	string value;
	if (getenv("PORT") != nullptr && !server.isBound()) {
		value = getenv("PORT");
		if (checkPort(value)) {
			server.bindSocket(value);
		}
	}
	if (getenv("PASSWORD") != nullptr && server.getPassword().empty()) {
		value = getenv("PASSWORD");
		if (checkPassword(value)) {
			server.setPassword(value);
		}
	}
	if (getenv("HOSTNAME") != nullptr && server.getHostname().empty()) {
		value = getenv("HOSTNAME");
		if (checkHostname(value)) {
			server.setHostname(value);
		}
	}
	if (getenv("OPERATORS") != nullptr) {
		value = getenv("OPERATORS");
		std::istringstream iss(value);
		string oper;
		while (std::getline(iss, oper, ',')) {
			server.addOperator(oper);
		}
	}
}

bool getEnv() {
	std::ifstream file(".env");
	string line;
	string key;
	string value;

	checkEnv();

	if (file.is_open()) {
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			std::getline(iss, key, '=');
			std::getline(iss, value);
			if (key == "PORT" && !server.isBound()) {
				if (checkPort(value)) {
					server.bindSocket(value);
				}
			} else if (key == "PASSWORD" && server.getPassword().empty()) {
				if (checkPassword(value)) {
					server.setPassword(value);
				}
			} else if (key == "HOSTNAME") {
				if (checkHostname(value)) {
					server.setHostname(value);
				}
			} else if (key == "OPERATORS") {
				std::istringstream iss(value);
				string oper;
				while (std::getline(iss, oper, ',')) {
					server.addOperator(oper);
				}
			}
		}
		file.close();
	} else if (file.bad()) {
		cerr << "Error: Unable to open .env file this is not fatal using provided arguments & defaults" << '\n';
		return false;
	}
	return true;
}
