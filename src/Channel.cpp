#include "Channel.hpp"

#include <iostream>

#include "ChannelMember.hpp"

Channel::Channel(const string &name) : name(name), password(), modes(0) {}

Channel::~Channel() {}

string Channel::getName() { return this->name; }

void Channel::setName(const string &name) { this->name = name; }

void Channel::addUser(const User &user) { this->members.push_back(ChannelMember(user)); }

void Channel::removeUser(User &user) {
	for (auto it = this->members.begin(); it != this->members.end(); it++) {
		if (it->getUsername() == user.getUsername()) {
			this->members.erase(it);
			break;
		}
	}
}

bool Channel::hasUser(User &user) {
	for (auto it = this->members.begin(); it != this->members.end(); it++) {
		if (it->getUsername() == user.getUsername()) {
			return true;
		}
	}
	return false;
}

std::vector<ChannelMember> &Channel::getMembers() { return this->members; }

void Channel::setModes(unsigned int modes) { this->modes = modes; }

unsigned int Channel::getModes() { return this->modes; }

void Channel::addModes(unsigned int modes) { this->modes |= modes; }

void Channel::removeModes(unsigned int modes) { this->modes &= ~modes; }

bool Channel::hasModes(unsigned int modes) { return (this->modes & modes) == modes; }

void Channel::printModes() {
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
