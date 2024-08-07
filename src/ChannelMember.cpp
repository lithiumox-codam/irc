#include "ChannelMember.hpp"

#include <iostream>

ChannelMember::ChannelMember(const User &user) : User(user), modes(0) {}

void ChannelMember::setModes(unsigned int modes) { this->modes = modes; }

auto ChannelMember::getModes() const -> unsigned int { return this->modes; }

void ChannelMember::addModes(unsigned int modes) { this->modes |= modes; }

void ChannelMember::removeModes(unsigned int modes) { this->modes &= ~modes; }

auto ChannelMember::hasModes(unsigned int modes) const -> bool { return (this->modes & modes) == modes; }

void ChannelMember::printModes() {
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
