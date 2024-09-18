#include <fstream>
#include <sstream>

#include "Server.hpp"

extern Server server;

void getDotEnv() {
	std::ifstream file(".env");
	std::string line;
	std::string key;
	std::string value;

	if (file.is_open()) {
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			std::getline(iss, key, '=');
			std::getline(iss, value);
			if (key == "PORT") {
				server.bindSocket(value);
			} else if (key == "PASSWORD") {
				server.setPassword(value);
			} else if (key == "HOSTNAME") {
				server.setHostname(value);
			}
		}
		file.close();
	}
}
