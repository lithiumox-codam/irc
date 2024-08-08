#include "ChannelMember.hpp"

#include <iostream>

ChannelMember::ChannelMember(User &&user) : User(std::move(user)), modes(0) {}

ChannelMember::ChannelMember(ChannelMember &&member) noexcept : User(std::move(member)), modes(member.modes) {}

auto ChannelMember::operator=(ChannelMember &&member) noexcept -> ChannelMember & {
	User::operator=(std::move(member));
	this->modes = member.modes;
	return *this;
}

void ChannelMember::setModes(unsigned int modes) { this->modes = modes; }

auto ChannelMember::getModes() const -> unsigned int { return this->modes; }

void ChannelMember::addModes(unsigned int modes) { this->modes |= modes; }

void ChannelMember::removeModes(unsigned int modes) { this->modes &= ~modes; }

auto ChannelMember::hasModes(unsigned int modes) const -> bool { return (this->modes & modes) == modes; }

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
