#pragma once

#include <vector>

#include "channelMember.hpp"

class Channel {
   private:
	vector<channelMember> members;
	std::string name;

   public:
	Channel(const std::string &name);
	~Channel();
	std::string getName();
	void setName(const std::string &name);
	void addUser(const User &user);
	void removeUser(User &user);
	bool hasUser(User &user);
	std::vector<channelMember> &getMembers();
};
