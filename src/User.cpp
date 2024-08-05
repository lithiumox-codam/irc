#include "User.hpp"

User::User(int socket) : username(""), nickname(""), realname(""), hostname(""), socket(socket), handshake(0) {}

User::User(const string &username, const string &realname, const string &hostname, int socket)
	: username(username), realname(realname), hostname(hostname), socket(socket) {}

User::~User() {}

int User::getSocket() const { return this->socket; }

const string &User::getUsername() { return this->username; }

const string &User::getNickname() { return this->nickname; }

void User::setNickname(const string &nickname) { this->nickname = nickname; }

void User::addHandshake(unsigned int handshake) { this->handshake |= handshake; }

unsigned int User::getHandshake() { return this->handshake; }

bool User::hasHandshake(unsigned int handshake) { return (this->handshake & handshake) == handshake; }
