#include "Channel.hpp"

#include <iostream>
#include <utility>

#include "ChannelMember.hpp"

Channel::Channel(string name) : name(std::move(name)), modes(0) {}

Channel::~Channel() = default;

auto Channel::getName() -> string { return this->name; }

void Channel::setName(const string &name) { this->name = name; }

void Channel::addUser(const User &user) { this->members.emplace_back(user); }

void Channel::removeUser(User &user) {
	for (auto it = this->members.begin(); it != this->members.end(); ++it) {
		if (it->getSocket() == user.getSocket()) {
			this->members.erase(it);
			break;
		}
	}
}

auto Channel::hasUser(User &user) -> bool {
	for (const auto &member : this->members) {
		if (member.getSocket() == user.getSocket()) {
			return true;
		}
	}
	return false;
}

auto Channel::getMembers() -> std::vector<ChannelMember> & { return this->members; }

void Channel::setModes(unsigned int modes) { this->modes = modes; }

auto Channel::getModes() const -> unsigned int { return this->modes; }

void Channel::addModes(unsigned int modes) { this->modes |= modes; }

void Channel::removeModes(unsigned int modes) { this->modes &= ~modes; }

auto Channel::hasModes(unsigned int modes) const -> bool { return (this->modes & modes) == modes; }

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
