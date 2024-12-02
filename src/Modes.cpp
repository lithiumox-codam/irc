#include "Modes.hpp"

#include "Server.hpp"

extern Server server;

Modes::Modes(Type type) : modes(0) { this->type = type; }

/**
 * @brief Construct a new Modes object
 *
 * @param modes The modes to set.
 */
Modes::Modes(unsigned int modes, Type type) : modes(modes), type(type) {}

/**
 * @brief Copy constructor.
 *
 * @param modes The modes to copy.
 */
Modes::Modes(const Modes &modes) noexcept : modes(modes.modes), type(modes.type) {}

/**
 * @brief Copy assignment operator.
 *
 * @param modes The modes to copy.
 * @return Modes&
 */
auto Modes::operator=(const Modes &modes) noexcept -> Modes & {
	this->modes = modes.modes;
	this->type = modes.type;
	return *this;
}

/**
 * @brief Sets the modes.
 * @warning This function will override the current modes.
 * @param modes The modes to set.
 */
void Modes::setModes(unsigned int modes) { this->modes = modes; }

/**
 * @brief Returns the modes.
 *
 * @return unsigned int
 */
unsigned int Modes::get() const { return modes; }

/**
 * @brief Adds one or more modes to the class. This function will only add the modes that are allowed for the user type.
 * For example, a channel user cannot have the M_INVISIBLE mode. And a user cannot have the M_MODERATED mode.
 *
 * @param modes The modes to add.
 */
void Modes::add(unsigned int modes) { this->modes |= modes; }

/**
 * @brief Removes one or more modes from the class. This function will only remove the modes that are allowed for the
 * user or channel. For example, a channel user cannot have the M_INVISIBLE mode. And a user cannot have the M_MODERATED
 * mode.
 *
 * @param modes The modes to remove.
 */
void Modes::remove(unsigned int modes) { this->modes &= ~modes; }

/**
 * @brief Checks if the user has one or more modes.
 * @example has(M_OPERATOR | M_VOICE) will return true if the user has both modes.
 *
 * @param modes The modes to check for.
 * @return true if the user has the modes
 * @return false if the user does not have the modes
 */
bool Modes::has(unsigned int modes) const { return (this->modes & modes) == modes; }

/**
 * @brief Returns a string representation of the modes.
 * @return string
 */
string Modes::getString() const {
	string result;
	const pair<unsigned int, char> modePairs[] = {{M_OPERATOR, 'o'},   {M_VOICE, 'v'},		 {M_INVISIBLE, 'i'},
												  {M_MODERATED, 'm'},  {M_INVITE_ONLY, 'i'}, {M_PASSWORD, 'k'},
												  {M_TOPIC_LOCK, 't'}, {M_LIMIT, 'l'}};

	for (const auto &modePair : modePairs) {
		if (has(modePair.first)) {
			result.push_back(modePair.second);
		}
	}
	return result;
}

/**
 * @brief Clears all modes.
 */
void Modes::clear() { modes = 0; }

/**
 * @brief Returns the type of the modes.
 *
 * @return Type
 */
Type Modes::getType() const { return type; }

/**
 * @brief Helper function to find and apply mode changes for a specific mode type
 *
 * @param modeChar The mode character to look for
 * @param modePairs The array of mode pairs to search in
 * @param addMode Whether to add or remove the mode
 * @return true if mode was found and applied, false otherwise
 */
bool Modes::changeHelper(char modeChar, const auto &modePairs, bool addMode) {
	const auto *const iter =
		ranges::find_if(modePairs, [modeChar](const auto &pair) { return pair.second == modeChar; });
	if (iter != modePairs.end()) {
		addMode ? add(iter->first) : remove(iter->first);
		return true;
	}
	return false;
}

/**
 * @brief Applies mode changes to the modes. This function will only apply the modes that are allowed for the type
 * specified in the constructor.
 *
 * @param modeChanges The mode changes to apply. ex: "+ov" or "-o+v"
 * @return string The unsupported modes.
 */
string Modes::applyChanges(const string &modeChanges) {
	string unsupportedModes;
	bool addMode = false;

	for (char modeChar : modeChanges) {
		if (modeChar == '+' || modeChar == '-') {
			addMode = (modeChar == '+');
			continue;
		}

		bool modeApplied = false;

		if (type == Type::USER) {
			modeApplied = changeHelper(modeChar, userModePairs, addMode);
		} else if (type == Type::CHANNEL) {
			modeApplied = changeHelper(modeChar, channelModePairs, addMode);
		} else if (type == Type::CHANNELMEMBER) {
			modeApplied = changeHelper(modeChar, channelMemberPairs, addMode);
		}

		if (!modeApplied) {
			unsupportedModes.push_back(modeChar);
		}
	}
	return unsupportedModes;
}

/**
 * @brief Overloaded stream operator.
 *
 * @param stream The stream to output to.
 * @param modes The modes to output.
 * @return ostream&
 */
ostream &operator<<(ostream &stream, const Modes &modes) {
	stream << modes.getString();
	return stream;
}
