#include <fstream>
#include <iostream>
#include <sstream>

#include "Server.hpp"

extern Server server;

static void checkEnv() {
	string value;
	if (getenv("PORT") != nullptr && !server.isBound()) {
		value = getenv("PORT");
		server.bindSocket(value);
	}
	if (getenv("PASSWORD") != nullptr && server.getPassword().empty()) {
		value = getenv("PASSWORD");
		server.setPassword(value);
	}
	if (getenv("HOSTNAME") != nullptr && server.getHostname().empty()) {
		value = getenv("HOSTNAME");
		server.setHostname(value);
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
				server.bindSocket(value);
			} else if (key == "PASSWORD" && server.getPassword().empty()) {
				server.setPassword(value);
			} else if (key == "HOSTNAME") {
				server.setHostname(value);
			}
		}
		file.close();
	} else if (file.bad()) {
		cerr << "Error: Unable to open .env file this is not fatal using provided arguments & defaults" << '\n';
		return false;
	}
	return true;
}
