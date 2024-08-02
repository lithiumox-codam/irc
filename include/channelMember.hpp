#pragma once

#include <type_traits>

#include "./User.hpp"

unsigned int const PERMISSIONS_NONE = 1 << 0;
unsigned int const PERMISSIONS_READ = 1 << 1;
unsigned int const PERMISSIONS_WRITE = 1 << 2;
unsigned int const PERMISSIONS_MANAGE = 1 << 3;
unsigned int const PERMISSIONS_BAN = 1 << 4;
unsigned int const PERMISSIONS_KICK = 1 << 5;

class channelMember : public User {
   private:
	unsigned int permissions;

   public:
	channelMember(const User &user);
	~channelMember();
	void setPermissions(unsigned int permissions);
	unsigned int getPermissions();
	void addPermissions(unsigned int permissions);
	void removePermissions(unsigned int permissions);
	bool hasPermissions(unsigned int permissions);
	void printPermissions();
	void clearPermissions();
};
