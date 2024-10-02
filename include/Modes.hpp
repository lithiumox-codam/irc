#pragma once

#include <string>

using namespace std;

/** Determines if the user is a channel operator. (will be defaulted to true when the user is a server operator) */
unsigned int const M_OPERATOR = 1 << 1;
/** Determines if the user can speak when the channel is moderated. */
unsigned int const M_VOICE = 1 << 2;
/** Determines if the user is invisible to other users. */
unsigned int const M_INVISIBLE = 1 << 3;

/**
 * Channel only modes
 */

/** Determines if the channel is moderated. */
unsigned int const M_MODERATED = 1 << 4;
/** Determines if the channel is invite only. */
unsigned int const M_INVITE_ONLY = 1 << 5;
/** Determines if the channel has a password. */
unsigned int const M_PASSWORD = 1 << 6;

class Modes {
   private:
	unsigned int modes;

   public:
	Modes();
	Modes(unsigned int modes);
	Modes(const Modes &modes) noexcept;
	auto operator=(const Modes &modes) noexcept -> Modes &;

	void setModes(unsigned int modes);
	[[nodiscard]] unsigned int getModes() const;
	void addModes(unsigned int modes);
	void removeModes(unsigned int modes);
	[[nodiscard]] bool hasModes(unsigned int modes) const;
	string printModes() const;
	void clearModes();
};
