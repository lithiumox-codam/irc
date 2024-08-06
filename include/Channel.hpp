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
	Channel(string name);
	~Channel();
	auto getName() -> string;
	void setName(const std::string &name);
	void addUser(const User &user);
	void removeUser(User &user);
	auto hasUser(User &user) -> bool;
	auto getMembers() -> vector<ChannelMember> &;
	void setModes(unsigned int modes);
	[[nodiscard]] auto getModes() const -> unsigned int;
	void addModes(unsigned int modes);
	void removeModes(unsigned int modes);
	[[nodiscard]] auto hasModes(unsigned int modes) const -> bool;
	void printModes();
};
