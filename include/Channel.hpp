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
	Channel(string &name);
	Channel(Channel &channel) = delete;
	auto operator=(Channel &channel) -> Channel & = delete;
	Channel(Channel &&channel) noexcept;
	auto operator=(Channel &&channel) noexcept -> Channel &;

	[[nodiscard]] const string & getName() const;
	void setName(const std::string &name);

	void addUser(User &&user);
	void removeUser(User &user);
	bool hasUser(User &user) const;

	[[nodiscard]] vector<ChannelMember> & getMembers() ;

	void setModes(unsigned int modes);
	[[nodiscard]] unsigned int getModes() const;
	void addModes(unsigned int modes);
	void removeModes(unsigned int modes);
	[[nodiscard]] bool hasModes(unsigned int modes) const;
	void printModes() const;
};
