#include "Modes.hpp"

#include "Server.hpp"

extern Server server;

Modes::Modes(Type type) : modes(0) { this->type = type; }

/**
 * @brief Construct a new Modes object
 *
 * @param modes The modes to set.
 */
Modes::Modes(unsigned int modes) : modes(modes) {}

/**
 * @brief Copy constructor.
 *
 * @param modes The modes to copy.
 */
Modes::Modes(const Modes &modes) noexcept : modes(modes.modes) {}

/**
 * @brief Copy assignment operator.
 *
 * @param modes The modes to copy.
 * @return Modes&
 */
auto Modes::operator=(const Modes &modes) noexcept -> Modes & {
	this->modes = modes.modes;
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
unsigned int Modes::getModes() const { return modes; }

/**
 * @brief Adds one or more modes to the class. This function will only add the modes that are allowed for the user type.
 * For example, a channel user cannot have the M_INVISIBLE mode. And a user cannot have the M_MODERATED mode.
 *
 * @param modes The modes to add.
 */
void Modes::addModes(unsigned int modes) {
	if (this->type == Type::CHANNEL) {
		modes &= M_MODERATED | M_INVITE_ONLY | M_PASSWORD | M_TOPIC_LOCK | M_LIMIT;
	}
	if (this->type == Type::USER) {
		modes &= M_OPERATOR | M_VOICE | M_INVISIBLE;
	}
	this->modes |= modes;
}

/**
 * @brief Removes one or more modes from the class. This function will only remove the modes that are allowed for the
 * user or channel. For example, a channel user cannot have the M_INVISIBLE mode. And a user cannot have the M_MODERATED
 * mode.
 *
 * @param modes The modes to remove.
 */
void Modes::removeModes(unsigned int modes) {
	if (this->type == Type::CHANNEL) {
		modes &= M_MODERATED | M_INVITE_ONLY | M_PASSWORD | M_TOPIC_LOCK | M_LIMIT;
	}
	if (this->type == Type::USER) {
		modes &= M_OPERATOR | M_VOICE | M_INVISIBLE;
	}
	this->modes &= ~modes;
}

/**
 * @brief Checks if the user has one or more modes.
 * @example hasModes(M_OPERATOR | M_VOICE) will return true if the user has both modes.
 *
 * @param modes The modes to check for.
 * @return true if the user has the modes
 * @return false if the user does not have the modes
 */
bool Modes::hasModes(unsigned int modes) const { return (this->modes & modes) == modes; }

/**
 * @brief Returns a string representation of the modes.
 * @return string
 */
string Modes::getModesString() const {
	string result;
	const pair<unsigned int, char> modePairs[] = {{M_OPERATOR, 'o'},   {M_VOICE, 'v'},		 {M_INVISIBLE, 'i'},
												  {M_MODERATED, 'm'},  {M_INVITE_ONLY, 'i'}, {M_PASSWORD, 'k'},
												  {M_TOPIC_LOCK, 't'}, {M_LIMIT, 'l'}};

	for (const auto &modePair : modePairs) {
		if (hasModes(modePair.first)) {
			result += modePair.second;
		}
	}
	return result;
}

/**
 * @brief Clears all modes.
 */
void Modes::clearModes() { modes = 0; }

/**
 * @brief Returns the type of the modes.
 *
 * @return Type
 */
Type Modes::getType() const { return type; }

/**
 * @brief Applies mode changes to the modes. This function will only apply the modes that are allowed for the type
 * specified in the constructor. For example, a channel user cannot have the M_INVISIBLE mode. And a user cannot have
 * the M_MODERATED mode.
 *
 * @param modeChanges The mode changes to apply. ex: "+ov" or "-o+v"
 * @return string The unsupported modes.
 */
string Modes::applyModeChanges(const string &modeChanges) {
	string unsupportedModes;
	bool addMode = false;

	for (char modeChar : modeChanges) {
		if (modeChar == '+' || modeChar == '-') {
			addMode = (modeChar == '+');
			continue;
		}

		if (type == Type::USER) {
			const auto *const iter =
				ranges::find_if(userModePairs, [modeChar](const auto &pair) { return pair.second == modeChar; });
			if (iter != userModePairs.end()) {
				addMode ? addModes(iter->first) : removeModes(iter->first);
				continue;
			}
		} else {
			const auto *const iter =
				ranges::find_if(channelModePairs, [modeChar](const auto &pair) { return pair.second == modeChar; });
			if (iter != channelModePairs.end()) {
				addMode ? addModes(iter->first) : removeModes(iter->first);
				continue;
			}
		}

		unsupportedModes.push_back(modeChar);
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
	stream << modes.getModesString();
	return stream;
}
