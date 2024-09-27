#include "ChannelMember.hpp"

#include <iostream>

ChannelMember::ChannelMember(const User &user) : User(user), modes(0) {}

ChannelMember::ChannelMember(const ChannelMember &member) noexcept : User(member), modes(member.modes) {}

auto ChannelMember::operator=(const ChannelMember &member) noexcept -> ChannelMember & {
	User::operator=(member);
	this->modes = member.modes;
	return *this;
}

void ChannelMember::setModes(unsigned int modes) { this->modes = modes; }

unsigned int ChannelMember::getModes() const { return this->modes; }

void ChannelMember::addModes(unsigned int modes) { this->modes |= modes; }

void ChannelMember::removeModes(unsigned int modes) { this->modes &= ~modes; }

bool ChannelMember::hasModes(unsigned int modes) const { return (this->modes & modes) == modes; }

void ChannelMember::printModes() const {
	cout << "Modes for " << this->getUsername() << ":" << "\n";
	if (this->hasModes(M_INVISIBLE)) {
		cout << "  +i (invisible)" << "\n";
	}
	if (this->hasModes(M_OPERATOR)) {
		cout << "  +o (operator)" << "\n";
	}
	if (this->hasModes(M_VOICE)) {
		cout << "  +v (voice)" << "\n";
	}
}

void ChannelMember::clearModes() { this->modes = 0; }
