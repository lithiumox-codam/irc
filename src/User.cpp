#include "User.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <vector>

#include "General.hpp"

User::User(int socket) : socket(socket), handshake(0) {}

User::~User() { close(this->socket); }

auto User::getSocket() const -> int { return this->socket; }

auto User::getUsername() -> const string & { return this->username; };

auto User::getNickname() -> const string & { return this->nickname; }

void User::setNickname(const string &nickname) { this->nickname = nickname; }

void User::setUsername(const string &username) { this->username = username; }

void User::setRealname(const string &realname) { this->realname = realname; }

void User::setHostname(const string &hostname) { this->hostname = hostname; }

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
	if (this->hasHandshake(U_WELCOME)) {
		cout << "U_WELCOME ";
	}
	cout << "]" << "\n";
}

auto User::getHandshake() const -> unsigned int { return this->handshake; }

auto User::hasHandshake(unsigned int handshake) const -> bool { return (this->handshake & handshake) == handshake; }

void User::printUser() {
	cout << "======================" << "\n";
	cout << setw(10) << left << "Username:" << this->username << "\n";
	cout << setw(10) << left << "Nickname:" << this->nickname << "\n";
	cout << setw(10) << left << "Realname:" << this->realname << "\n";
	cout << setw(10) << left << "Hostname:" << this->hostname << "\n";
	cout << setw(10) << left << "Socket:" << this->socket << "\n";
	this->printHandshake();
	cout << "======================" << "\n";
}

auto User::checkPacket() const -> bool {
	if (this->context.empty()) {
		return false;
	}
	if (!this->context.ends_with("\r\n")) {
		return false;
	}

	Packet packet = parse(this->context);

	for (auto &pair : packet) {
		cout << pair.first << "\t" << pair.second << "\n";
	}

	packetProcessor(packet, this->socket);

	return true;
}

auto User::readFromSocket() -> int {
	vector<char> buffer(UserConfig::BUFFER_SIZE);
	int bytesRead = recv(this->socket, buffer.data(), buffer.size(), 0);

	if (bytesRead == -1) {
		cerr << "Error: recv failed" << "\n";
		return 1;
	}

	if (bytesRead == 0) {
		cerr << "Error: client disconnected" << "\n";
		return 2;
	}

	buffer.push_back('\0');

	this->context.append(buffer.data());

	cout << "Buffer for socket " << this->socket << ": " << this->context << "\n";

	// This needs to only clear unti /r/n
	if (this->checkPacket()) {
		this->context.clear();
	}
	return 0;
}
