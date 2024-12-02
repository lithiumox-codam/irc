#pragma once

#include <array>
#include <cstdint>
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

static constexpr array<pair<unsigned int, char>, 3> channelMemberPairs{{{M_OPERATOR, 'o'}, {M_VOICE, 'v'}}};

static constexpr array<pair<unsigned int, char>, 2> userModePairs{{{M_OPERATOR, 'o'}, {M_INVISIBLE, 'i'}}};

static constexpr array<pair<unsigned int, char>, 5> channelModePairs{
	{{M_MODERATED, 'm'}, {M_INVITE_ONLY, 'i'}, {M_PASSWORD, 'k'}, {M_TOPIC_LOCK, 't'}, {M_LIMIT, 'l'}}};

enum class Type : uint8_t { USER, CHANNEL, CHANNELMEMBER };

class Modes {
   private:
	unsigned int modes;
	Type type;

   public:
	Modes(Type type);
	~Modes() = default;
	Modes(unsigned int modes, Type type);
	Modes(const Modes &modes) noexcept;
	auto operator=(const Modes &modes) noexcept -> Modes &;

	void setModes(unsigned int modes);
	[[nodiscard]] unsigned int get() const;
	void add(unsigned int modes);
	void remove(unsigned int modes);
	[[nodiscard]] bool has(unsigned int modes) const;
	string getString() const;
	void clear();
	Type getType() const;
	bool changeHelper(char modeChar, const auto &modePairs, bool addMode);
	string applyChanges(const string &modeChanges);

	friend ostream &operator<<(ostream &stream, const Modes &modes);
};
