#include "../include/channelMember.hpp"

channelMember::channelMember(const User &user) : User(user) { this->permissions = PERMISSIONS_NONE; }

channelMember::~channelMember() {}

void channelMember::setPermissions(unsigned int permissions) { this->permissions = permissions; }

unsigned int channelMember::getPermissions() { return this->permissions; }

void channelMember::addPermissions(unsigned int permissions) { this->permissions |= permissions; }

void channelMember::removePermissions(unsigned int permissions) { this->permissions &= ~permissions; }

bool channelMember::hasPermissions(unsigned int permissions) {
	return (this->permissions & permissions) == permissions;
}

void channelMember::clearPermissions() { this->permissions = PERMISSIONS_NONE; }
