#include "Channel.hpp"

#include <iostream>
#include <utility>

#include "IRStream.hpp"

using namespace std;

Channel::Channel() : modes(0) {};

Channel::Channel(const string &name) : name(name), modes(0) {}

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

void Channel::addUser(User *user) { this->members.push_back(make_pair(user, Modes())); }

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

void Channel::setModes(unsigned int modes) { this->modes = modes; }

unsigned int Channel::getModes() const { return this->modes; }

void Channel::addModes(unsigned int modes) { this->modes |= modes; }

void Channel::removeModes(unsigned int modes) { this->modes &= ~modes; }

bool Channel::hasModes(unsigned int modes) const { return (this->modes & modes) == modes; }

void Channel::printModes() const {
	if (hasModes(M_MODERATED)) {
		cout << "m";
	}
	if (hasModes(M_INVITE_ONLY)) {
		cout << "i";
	}
	if (hasModes(M_PASSWORD)) {
		cout << "k";
	}
	cout << "\n";
}

void Channel::broadcast(User *user, const string &message) {
	IRStream stream;
	stream.prefix(user).param("PRIVMSG").param(this->getName()).trail(message).end();
	for (auto &member : *this->getMembers()) {
		if (member.first->getSocket() == user->getSocket()) {
			continue;
		}
		stream.sendPacket(member.first);
	}
}

const string &Channel::getTopic() const { return this->topic; }

void Channel::setTopic(const string &topic) { this->topic = topic; }

ostream &operator<<(ostream &stream, Channel &channel) {
	stream << "Channel: " << channel.getName() << "\n";
	stream << "Password: " << channel.getPassword() << "\n";
	stream << "Modes: " << channel.getModes() << "\n";
	stream << "Members: " << "\n";
	auto *members = channel.getMembers();
	for (const auto &member : *members) {
		stream << member.first->getNickname() << '{' + member.second.printModes() + '}' << "\n";
	}
	return stream;
}
