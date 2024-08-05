#include "User.hpp"
#include "General.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <iomanip>
#include <iostream>

User::User(int socket) : username(""), nickname(""), realname(""), hostname(""), socket(socket), handshake(0), context("") {}

User::User(const string &username, const string &realname, const string &hostname, int socket)
	: username(username), realname(realname), hostname(hostname), socket(socket), handshake(0), context("") {}

User::~User() { close(this->socket); }

int User::getSocket() const { return this->socket; }

const string &User::getUsername() { return this->username; }

const string &User::getNickname() { return this->nickname; }

void User::setNickname(const string &nickname) { this->nickname = nickname; }

void User::addHandshake(unsigned int handshake) { this->handshake |= handshake; }

void User::printHandshake() {
	cout << "Handshake contains: [";
	if (this->hasHandshake(U_USER)) cout << "U_USER ";
	if (this->hasHandshake(U_NICK)) cout << "U_NICK ";
	if (this->hasHandshake(U_AUTHENTICATED)) cout << "U_AUTHENTICATED ";
	if (this->hasHandshake(U_WELCOME)) cout << "U_WELCOME ";
	cout << "]" << endl;
}

unsigned int User::getHandshake() { return this->handshake; }

bool User::hasHandshake(unsigned int handshake) { return (this->handshake & handshake) == handshake; }

void User::printUser() {
	cout << "======================" << endl;
	cout << setw(10) << left << "Username:" << this->username << endl;
	cout << setw(10) << left << "Nickname:" << this->nickname << endl;
	cout << setw(10) << left << "Realname:" << this->realname << endl;
	cout << setw(10) << left << "Hostname:" << this->hostname << endl;
	cout << setw(10) << left << "Socket:" << this->socket << endl;
	this->printHandshake();
	cout << "======================" << endl;
}

bool User::checkPacket() const {
	if (this->context.empty()) return false;
	if (!this->context.ends_with("\r\n")) return false;

	Packet packet = parse(this->context);

	for (auto &pair : packet) {
		cout << pair.first << "\t" << pair.second << endl;
	}

	PacketProcessor(packet, this->socket);

	return true;
}

int User::readFromSocket() {
	char buffer[1024];
	int bytesRead = recv(this->socket, buffer, 1024, 0);

	if (bytesRead == -1) {
		cerr << "Error: recv failed" << endl;
		return 1;
	}

	if (bytesRead == 0) {
		cerr << "Error: client disconnected" << endl;
		return 2;
	}

	buffer[bytesRead] = '\0';

	this->context.append(buffer);

	cout << "Buffer for socket " << this->socket << ": " << this->context << endl;

	// This needs to only clear unti /r/n
	if (this->checkPacket() == true) {
		this->context.clear();
	}
	return 0;
}
