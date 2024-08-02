#include "../include/User.hpp"

User::User(const string &username, const string &nick) : username(username), nickname(nick) {}

User::~User() {}

const string &User::getUsername() { return this->username; }

const string &User::getNickname() { return this->nickname; }

void User::setNickname(const string &nickname) { this->nickname = nickname; }
