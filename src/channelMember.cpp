#include "ChannelMember.hpp"

#include <iostream>

ChannelMember::ChannelMember(const User &user) : User(user) { this->permissions = 0; }

ChannelMember::~ChannelMember() {}

void ChannelMember::setPermissions(unsigned int permissions) { this->permissions = permissions; }

unsigned int ChannelMember::getPermissions() { return this->permissions; }

void ChannelMember::addPermissions(unsigned int permissions) { this->permissions |= permissions; }

void ChannelMember::removePermissions(unsigned int permissions) { this->permissions &= ~permissions; }

bool ChannelMember::hasPermissions(unsigned int permissions) {
	return (this->permissions & permissions) == permissions;
}

void ChannelMember::printPermissions() {
	cout << "Permissions for " << this->getUsername() << ":" << endl;
	if (this->hasPermissions(P_READ)) {
		std::cout << "  Read permissions" << std::endl;
	}
	if (this->hasPermissions(P_WRITE)) {
		std::cout << "  Write permissions" << std::endl;
	}
	if (this->hasPermissions(P_MANAGE)) {
		std::cout << "  Manage permissions" << std::endl;
	}
	if (this->hasPermissions(P_BAN)) {
		std::cout << "  Ban permissions" << std::endl;
	}
	if (this->hasPermissions(P_KICK)) {
		std::cout << "  Kick permissions" << std::endl;
	}
}

void ChannelMember::clearPermissions() { this->permissions = 0; }
