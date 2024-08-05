#include "User.hpp"

#include <iomanip>
#include <iostream>

User::User(const string &username, const string &realname, const string &hostname, int socket)
	: username(username), realname(realname), hostname(hostname), socket(socket), handshake(0) {}

User::~User() {}

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
