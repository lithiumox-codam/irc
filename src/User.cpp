#include "User.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <iomanip>
#include <iostream>

#include "General.hpp"

User::User(int socket)
	: username(""), nickname(""), realname(""), hostname(""), socket(socket), handshake(0), context("") {}

User::~User() { close(this->socket); }

int User::getSocket() const { return this->socket; }

const string &User::getUsername() { return this->username; }

const string &User::getNickname() { return this->nickname; }

void User::setNickname(const string &nickname) { this->nickname = nickname; }

void User::setUsername(const string &username) { this->username = username; }

void User::setRealname(const string &realname) { this->realname = realname; }

void User::setHostname(const string &hostname) { this->hostname = hostname; }

void User::addHandshake(unsigned int handshake) { this->handshake |= handshake; }

void User::printHandshake() {
	cout << "Handshake contains: [";
	if (this->hasHandshake(U_USER)) cout << "U_USER ";
	if (this->hasHandshake(U_NICK)) cout << "U_NICK ";
	if (this->hasHandshake(U_AUTHENTICATED)) cout << "U_AUTHENTICATED ";
	if (this->hasHandshake(U_WELCOME)) cout << "U_WELCOME ";
	cout << "]" << "\n";
}

unsigned int User::getHandshake() { return this->handshake; }

bool User::hasHandshake(unsigned int handshake) { return (this->handshake & handshake) == handshake; }

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

bool User::checkPacket() const {
	if (this->context.empty()) return false;
	if (!this->context.ends_with("\r\n")) return false;

	Packet packet = parse(this->context);

	for (auto &pair : packet) {
		cout << pair.first << "\t" << pair.second << "\n";
	}

	PacketProcessor(packet, this->socket);

	return true;
}

int User::readFromSocket() {
	char buffer[1024];
	int bytesRead = recv(this->socket, buffer, 1024, 0);

	if (bytesRead == -1) {
		cerr << "Error: recv failed" << "\n";
		return 1;
	}

	if (bytesRead == 0) {
		cerr << "Error: client disconnected" << "\n";
		return 2;
	}

	buffer[bytesRead] = '\0';

	this->context.append(buffer);

	cout << "Buffer for socket " << this->socket << ": " << this->context << "\n";

	// This needs to only clear unti /r/n
	if (this->checkPacket() == true) {
		this->context.clear();
	}
	return 0;
}
