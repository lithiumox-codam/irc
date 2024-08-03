#include "User.hpp"

User::User(const string &username, const string &nick) : username(username), nickname(nick) {}

User::~User() {}

const string &User::getUsername() { return this->username; }

const string &User::getNickname() { return this->nickname; }

void User::setNickname(const string &nickname) { this->nickname = nickname; }

void User::addHandshake(unsigned int handshake) { this->handshake |= handshake; }

unsigned int User::getHandshake() { return this->handshake; }

bool User::hasHandshake(unsigned int handshake) { return (this->handshake & handshake) == handshake; }
