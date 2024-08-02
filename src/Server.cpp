#include "../include/Server.hpp"

#include <cstring>
#include <string>

Server::Server() {
	char hostname[1024];
	try {
		gethostname(hostname, 1024);
		this->hostname = hostname;
		this->running = false;
		this->socket = 0;
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

static void receiveMessage(int client) {
	char buffer[BUFFER_SIZE + 1];

	cout << "Received message:" << endl;

	while (recv(client, buffer, BUFFER_SIZE, 0) > 0) {
		buffer[BUFFER_SIZE] = '\0';
		cout << buffer;
	}
	cout << endl;
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
	// Listen for incoming connections, with a backlog of 10 pending connections
	while (1) {
		// Accept a connection
		const int client = accept(this->socket, NULL, NULL);

		if (client == -1) {
			if (errno == EWOULDBLOCK) continue;

			cerr << strerror(errno) << endl;
			cerr << "Error: accept failed" << endl;
			exit(EXIT_FAILURE);
		} else {
			cout << "Connection accepted" << endl;
		}

		// Receive a message from the client
		receiveMessage(client);
	}
}

void Server::stop(void) {
	cout << "Server stopped" << endl;
	if (this->running) {
		close(this->socket);
		this->running = false;
	}
}
