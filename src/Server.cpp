#include "../include/Server.hpp"

#include <unistd.h>

Server::Server(const string &password, const string &port) {
	char hostname[1024];
	try {
		gethostname(hostname, 1024);
		this->password = password;
		this->port = stoi(port);
		this->hostname = hostname;
		this->running = false;
		this->socket = 0;
	} catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	this->start();
}

Server::~Server() { this->stop(); }

void Server::stop() {
	cout << "Server stopped" << endl;
	if (this->running) {
		close(this->socket);
		this->running = false;
	}
}

void Server::start() {
	struct sockaddr_in address;
	if (this->running) {
		std::cerr << "Server is already running" << std::endl;
		return;
	}

	if ((this->socket = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "Socket creation error" << std::endl;
		return;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(this->port);

	if (bind(this->socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
		std::cerr << "Bind failed" << std::endl;
		return;
	}

	if (listen(this->socket, 3) < 0) {
		std::cerr << "Listen failed" << std::endl;
		return;
	}

	this->running = true;
	cout << "Server started on " << this->hostname << ":" << this->port << endl;

	while (running) {
		int new_socket;
		int addrlen = sizeof(address);
		if ((new_socket = accept(this->socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
			std::cerr << "Accept failed" << std::endl;
			return;
		}

		char buffer[BUFFER_SIZE] = {0};
		read(new_socket, buffer, BUFFER_SIZE);
		cout << buffer << endl;
	}
}
