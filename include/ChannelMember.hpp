#pragma once

#include <type_traits>

#include "User.hpp"

/**
 * @brief These are the possible permissions for a channel member.
 *
 * P_READ: The user has read permissions. 0000 0010
 * P_WRITE: The user has write permissions 0000 0100
 * P_MANAGE: The user has manage permissions 0000 1000
 * P_BAN: The user has ban permissions 0001 0000
 * P_KICK: The user has kick permissions 0010 0000
 * P_ALL: The user has all permissions 0011 1110
 */

unsigned int const P_NONE = 1 << 0;
unsigned int const P_READ = 1 << 1;
unsigned int const P_WRITE = 1 << 2;
unsigned int const P_MANAGE = 1 << 3;
unsigned int const P_BAN = 1 << 4;
unsigned int const P_KICK = 1 << 5;
unsigned int const P_ALL = P_READ | P_WRITE | P_MANAGE | P_BAN | P_KICK;

class ChannelMember : public User {
   private:
	unsigned int permissions;

   public:
	ChannelMember(const User &user);
	~ChannelMember();
	void setPermissions(unsigned int permissions);
	unsigned int getPermissions();
	void addPermissions(unsigned int permissions);
	void removePermissions(unsigned int permissions);
	bool hasPermissions(unsigned int permissions);
	void printPermissions();
	void clearPermissions();
};
