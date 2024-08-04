#pragma once

#include <vector>

#include "ChannelMember.hpp"

/** Determines if the channel is moderated. */
unsigned int const M_MODERATED = 1 << 4;
/** Determines if the channel is invite only. */
unsigned int const M_INVITE_ONLY = 1 << 5;
/** Determines if the channel has a password. */
unsigned int const M_PASSWORD = 1 << 6;

class Channel {
   private:
	vector<ChannelMember> members;
	string name;
	string password;
	string topic;

	unsigned int modes;

   public:
	Channel(const string &name);
	~Channel();
	string getName();
	void setName(const std::string &name);
	void addUser(const User &user);
	void removeUser(User &user);
	bool hasUser(User &user);
	vector<ChannelMember> &getMembers();
	void setModes(unsigned int modes);
	unsigned int getModes();
	void addModes(unsigned int modes);
	void removeModes(unsigned int modes);
	bool hasModes(unsigned int modes);
	void printModes();
};
