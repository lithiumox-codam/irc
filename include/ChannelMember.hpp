#pragma once

#include "User.hpp"

/** Determines if the user is a channel operator. (will be defaulted to true when the user is a server operator) */
unsigned int const M_OPERATOR = 1 << 1;
/** Determines if the user can speak when the channel is moderated. */
unsigned int const M_VOICE = 1 << 2;
/** Determines if the user is invisible to other users. */
unsigned int const M_INVISIBLE = 1 << 3;

class ChannelMember : public User {
   private:
	unsigned int modes;

   public:
	ChannelMember(const User &user);

	void setModes(unsigned int modes);
	[[nodiscard]] auto getModes() const -> unsigned int;
	void addModes(unsigned int modes);
	void removeModes(unsigned int modes);
	[[nodiscard]] auto hasModes(unsigned int modes) const -> bool;
	void printModes() const;
	void clearModes();
};
