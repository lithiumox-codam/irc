#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "Modes.hpp"
#include "User.hpp"

using namespace std;

/** Determines if the channel is moderated. */
unsigned int const M_MODERATED = 1 << 4;
/** Determines if the channel is invite only. */
unsigned int const M_INVITE_ONLY = 1 << 5;
/** Determines if the channel has a password. */
unsigned int const M_PASSWORD = 1 << 6;

class Channel {
   private:
	vector<pair<User *, Modes>> members;
	string name;
	string password;
	string topic;

	unsigned int modes;

   public:
	Channel();
	Channel(const string &name);
	Channel(const Channel &channel) noexcept;
	Channel &operator=(const Channel &channel) noexcept;

	[[nodiscard]] const string &getName() const;
	void setName(const std::string &name);

	[[nodiscard]] const string &getPassword() const;
	void setPassword(const std::string &password);

	void addUser(User *user);
	void removeUser(User *user);
	bool hasUser(User *user) const;

	[[nodiscard]] vector<pair<User *, Modes>> *getMembers();

	void setModes(unsigned int modes);
	[[nodiscard]] unsigned int getModes() const;
	void addModes(unsigned int modes);
	void removeModes(unsigned int modes);
	[[nodiscard]] bool hasModes(unsigned int modes) const;
	void printModes() const;
	void broadcast(User *user, const string &message);
};

ostream &operator<<(ostream &stream, Channel &channel);
