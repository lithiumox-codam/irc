#include "../includes/irc.hpp"

IRC::Server::Server() {
	this->socket = 0;
	this->host = "localhost";
	this->port = 6667;
	this->name = "irc_server";
	this->password = "";
	this->motd = "Welcome to the server!";
	this->max_clients = 10;
}

IRC::Server::Server(const std::string &host, int port, const std::string &name, int max_clients, const std::string &password) {
	this->socket = 0;
	this->host = host;
	this->port = port;
	this->name = name;
	this->password = password;
	this->motd = "Welcome to the server!";
	this->max_clients = max_clients;
}

IRC::Server::~Server() {}

void IRC::Server::send(const std::string &message, Options options = Options()) {
	if (options == Options::ALL) {
		for (auto &client : this->clients) {
			client->send(message);
		}
	} else if (options == Options::EXCEPT) {
		for (auto &client : this->clients) {
			if (client->getSocket() != this->socket) {
				client->send(message);
			}
		}
	}
}
