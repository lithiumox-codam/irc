#include "Channel.hpp"

#include <iostream>
#include <utility>

#include "ChannelMember.hpp"

using namespace std;

Channel::Channel() : modes(0) {};

Channel::Channel(const string &name) : name(name), modes(0) {}

Channel::Channel(const Channel &channel) noexcept
	: members(channel.members),
	  name(channel.name),
	  password(channel.password),
	  topic(channel.password),
	  modes(channel.modes) {}

auto Channel::operator=(const Channel &channel) noexcept -> Channel & {
	this->members = channel.members;
	this->name = channel.name;
	this->password = channel.password;
	this->topic = channel.topic;
	this->modes = channel.modes;
	return *this;
}

const string &Channel::getName() const { return this->name; }

void Channel::setName(const string &name) { this->name = name; }

void Channel::addUser(User &user) { this->members.emplace_back(std::move(user)); }

void Channel::removeUser(User &user) {
	for (auto it = this->members.begin(); it != this->members.end(); ++it) {
		if (it->getSocket() == user.getSocket()) {
			this->members.erase(it);
			break;
		}
	}
}

bool Channel::hasUser(User &user) const {
	for (const auto &member : this->members) {
		if (member.getSocket() == user.getSocket()) {
			return true;
		}
	}
	return false;
}

std::vector<ChannelMember> &Channel::getMembers() { return this->members; }

void Channel::setModes(unsigned int modes) { this->modes = modes; }

unsigned int Channel::getModes() const { return this->modes; }

void Channel::addModes(unsigned int modes) { this->modes |= modes; }

void Channel::removeModes(unsigned int modes) { this->modes &= ~modes; }

bool Channel::hasModes(unsigned int modes) const { return (this->modes & modes) == modes; }

void Channel::printModes() const {
	cout << "Modes for " << this->getName() << ":" << "\n";
	if (this->hasModes(M_OPERATOR)) {
		cout << "  +o (operator)" << "\n";
	}
	if (this->hasModes(M_VOICE)) {
		cout << "  +v (voice)" << "\n";
	}
	if (this->hasModes(M_MODERATED)) {
		cout << "  +m (moderated)" << "\n";
	}
	if (this->hasModes(M_INVITE_ONLY)) {
		cout << "  +i (invite only)" << "\n";
	}
	if (this->hasModes(M_PASSWORD)) {
		cout << "  +k (password protected)" << "\n";
	}
}
