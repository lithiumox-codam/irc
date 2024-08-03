#include "Channel.hpp"

#include <iostream>

#include "ChannelMember.hpp"

Channel::Channel(const std::string &name) : name(name) {}

Channel::~Channel() {}

std::string Channel::getName() { return this->name; }

void Channel::setName(const std::string &name) { this->name = name; }

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
		cout << it->getUsername() << " " << user.getUsername() << endl;
		if (it->getUsername() == user.getUsername()) {
			return true;
		}
	}
	return false;
}

std::vector<ChannelMember> &Channel::getMembers() { return this->members; }
