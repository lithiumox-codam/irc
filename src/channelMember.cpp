#include "../include/channelMember.hpp"

#include <iostream>

channelMember::channelMember(const User &user) : User(user) { this->permissions = 0; }

channelMember::~channelMember() {}

void channelMember::setPermissions(unsigned int permissions) { this->permissions = permissions; }

unsigned int channelMember::getPermissions() { return this->permissions; }

void channelMember::addPermissions(unsigned int permissions) { this->permissions |= permissions; }

void channelMember::removePermissions(unsigned int permissions) { this->permissions &= ~permissions; }

bool channelMember::hasPermissions(unsigned int permissions) {
	return (this->permissions & permissions) == permissions;
}

void channelMember::printPermissions() {
	cout << "Permissions for " << this->getUsername() << ":" << endl;
	if (this->hasPermissions(PERMISSIONS_READ)) {
		std::cout << "  Read permissions" << std::endl;
	}
	if (this->hasPermissions(PERMISSIONS_WRITE)) {
		std::cout << "  Write permissions" << std::endl;
	}
	if (this->hasPermissions(PERMISSIONS_MANAGE)) {
		std::cout << "  Manage permissions" << std::endl;
	}
	if (this->hasPermissions(PERMISSIONS_BAN)) {
		std::cout << "  Ban permissions" << std::endl;
	}
	if (this->hasPermissions(PERMISSIONS_KICK)) {
		std::cout << "  Kick permissions" << std::endl;
	}
}

void channelMember::clearPermissions() { this->permissions = 0; }
