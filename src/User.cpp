#include "User.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <vector>

#include "General.hpp"
#include "Server.hpp"

extern Server server;

User::User(int socket) : socket(socket), handshake(0) {}

User::~User() { close(this->socket); }

auto User::getSocket() const -> int { return this->socket; }

auto User::getUsername() const -> const string & { return this->username; };

auto User::getNickname() const -> const string & { return this->nickname; }

void User::setNickname(string &nickname) { this->nickname = std::move(nickname); }

void User::setUsername(string &username) { this->username = std::move(username); }

void User::setRealname(string &realname) { this->realname = std::move(realname); }

void User::setHostname(string &hostname) { this->hostname = std::move(hostname); }

void User::addHandshake(unsigned int handshake) { this->handshake |= handshake; }

void User::printHandshake() const {
	cout << "Handshake contains: [";
	if (this->hasHandshake(U_USER)) {
		cout << "U_USER ";
	}
	if (this->hasHandshake(U_NICK)) {
		cout << "U_NICK ";
	}
	if (this->hasHandshake(U_AUTHENTICATED)) {
		cout << "U_AUTHENTICATED ";
	}
	if (this->hasHandshake(U_INFO)) {
		cout << "U_INFO ";
	}
	if (this->hasHandshake(U_WELCOME)) {
		cout << "U_WELCOME ";
	}
	cout << "]"
		 << "\n";
}

auto User::getHandshake() const -> unsigned int { return this->handshake; }

auto User::hasHandshake(unsigned int handshake) const -> bool { return (this->handshake & handshake) == handshake; }

void User::printUser() const {
	cout << "======================"
		 << "\n";
	cout << setw(10) << left << "Username:" << this->username << "\n";
	cout << setw(10) << left << "Nickname:" << this->nickname << "\n";
	cout << setw(10) << left << "Realname:" << this->realname << "\n";
	cout << setw(10) << left << "Hostname:" << this->hostname << "\n";
	cout << setw(10) << left << "Socket:" << this->socket << "\n";
	this->printHandshake();
	cout << "======================"
		 << "\n";
}

// auto User::checkPacket() -> bool {
// 	if (this->in_buffer.empty()) {
// 		return false;
// 	}
// 	if (!this->in_buffer.ends_with("\r\n")) {
// 		return false;
// 	}

// 	parse(*this);

// 	return true;
// }

auto User::readFromSocket() -> int {
	vector<char> buffer(UserConfig::BUFFER_SIZE);
	int bytesRead = recv(this->socket, buffer.data(), buffer.size(), 0);

	if (bytesRead == -1) {
		cerr << "Error: recv failed"
			 << "\n";
		return 1;
	}

	if (bytesRead == 0) {
		cerr << "Error: client disconnected"
			 << "\n";
		return 2;
	}

	buffer.push_back('\0');

	this->in_buffer.append(buffer.data());

	cout << "Buffer for socket " << this->socket << ": " << this->in_buffer << "\n";

	parse(*this);
	return 0;
}

auto User::getNextCommand() -> string {
	if (this->in_buffer.empty()) {
		throw runtime_error("Buffer is empty");
	}
	string command = this->in_buffer.substr(0, this->in_buffer.find("\r\n"));
	this->in_buffer.erase(0, this->in_buffer.find("\r\n") + 2);
	return command;
}
