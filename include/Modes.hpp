#pragma once

#include <string>

using namespace std;

/* Global modes */

/** Determines if a user is an operator server wide or in a channel. (+o) */
unsigned int const M_OPERATOR = 1 << 1;

/* User modes */

/** Determines if the user can speak when the channel is moderated. (+v) */
unsigned int const M_VOICE = 1 << 2;
/** Determines if the user is invisible to other users. (+i) */
unsigned int const M_INVISIBLE = 1 << 3;

/* Channel modes */

/** Determines if the channel is moderated. (+m) */
unsigned int const M_MODERATED = 1 << 4;
/** Determines if the channel is invite only. (+i) */
unsigned int const M_INVITE_ONLY = 1 << 5;
/** Determines if the channel has a password. (+k) */
unsigned int const M_PASSWORD = 1 << 6;
/** Determines if the channel topic is locked. (+t) */
unsigned int const M_TOPIC_LOCK = 1 << 7;
/** Determines if the channel has a limit. (+l) */
unsigned int const M_LIMIT = 1 << 8;

class Modes {
   private:
	unsigned int modes;

   public:
	Modes();
	~Modes() = default;
	Modes(unsigned int modes);
	Modes(const Modes &modes) noexcept;
	auto operator=(const Modes &modes) noexcept -> Modes &;

	void setModes(unsigned int modes);
	[[nodiscard]] unsigned int getModes() const;
	void addModes(unsigned int modes);
	void removeModes(unsigned int modes);
	[[nodiscard]] bool hasModes(unsigned int modes) const;
	string getModesString() const;
	void clearModes();
};
