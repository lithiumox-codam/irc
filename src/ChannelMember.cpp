#include "ChannelMember.hpp"

#include <iostream>

ChannelMember::ChannelMember(const User &user) : User(user), modes(0) {}

ChannelMember::~ChannelMember() {}

void ChannelMember::setModes(unsigned int modes) { this->modes = modes; }

unsigned int ChannelMember::getModes() { return this->modes; }

void ChannelMember::addModes(unsigned int modes) { this->modes |= modes; }

void ChannelMember::removeModes(unsigned int modes) { this->modes &= ~modes; }

bool ChannelMember::hasModes(unsigned int modes) { return (this->modes & modes) == modes; }

void ChannelMember::printModes() {
	cout << "Modes for " << this->getUsername() << ":" << endl;
	if (this->hasModes(M_INVISIBLE)) {
		cout << "  +i (invisible)" << endl;
	}
	if (this->hasModes(M_OPERATOR)) {
		cout << "  +o (operator)" << endl;
	}
	if (this->hasModes(M_VOICE)) {
		cout << "  +v (voice)" << endl;
	}
}

void ChannelMember::clearModes() { this->modes = 0; }
