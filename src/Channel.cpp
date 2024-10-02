#include "Channel.hpp"

#include <iostream>
#include <utility>

#include "IRStream.hpp"

using namespace std;

Channel::Channel() {};

Channel::Channel(const string &name) : name(name) {}

Channel::Channel(const Channel &channel) noexcept
	: members(channel.members),
	  name(channel.name),
	  password(channel.password),
	  topic(channel.password),
	  modes(channel.modes) {}

Channel &Channel::operator=(const Channel &channel) noexcept {
	this->members = channel.members;
	this->name = channel.name;
	this->password = channel.password;
	this->topic = channel.topic;
	this->modes = channel.modes;
	return *this;
}

const string &Channel::getName() const { return this->name; }

const string &Channel::getPassword() const { return this->password; }

void Channel::setPassword(const string &password) { this->password = password; }

void Channel::setName(const string &name) { this->name = name; }

/**
 * @brief Adds a user to the channel.
 * @remark The oldest user will be assigned operator status. Or the first user to join the channel.
 *
 * @param user The user to add.
 */
void Channel::addUser(User *user) {
	this->members.push_back(make_pair(user, Modes()));
	this->members.front().second.addModes(M_OPERATOR);
}

void Channel::removeUser(User *user) {
	// NOLINTNEXTLINE
	for (auto it = this->members.begin(); it != this->members.end(); ++it) {
		if (it->first->getSocket() == user->getSocket()) {
			this->members.erase(it);
			return;
		}
	}
}

bool Channel::hasUser(User *user) const {
	// NOLINTNEXTLINE
	for (const auto &member : this->members) {
		if (member.first->getSocket() == user->getSocket()) {
			return true;
		}
	}
	return false;
}

std::vector<pair<User *, Modes>> *Channel::getMembers() { return &this->members; }

void Channel::broadcast(User *user, const string &message) {
	IRStream stream;
	stream.prefix(user, this).param("PRIVMSG").param(this->getName()).trail(message).end();
	for (auto &member : *this->getMembers()) {
		if (member.first->getSocket() == user->getSocket()) {
			continue;
		}
		cout << stream.getString();
		stream.sendPacket(member.first);
	}
}

string Channel::getUserModes(User *user) {
	for (const auto &member : this->members) {
		if (member.first->getSocket() == user->getSocket()) {
			return member.second.getModesString();
		}
	}
	return "";
}

const string &Channel::getTopic() const { return this->topic; }

void Channel::setTopic(const string &topic) { this->topic = topic; }
