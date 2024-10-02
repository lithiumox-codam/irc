#include "Modes.hpp"

Modes::Modes() : modes(0) {}

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
 * @brief Adds one or more modes to the user.
 *
 * @param modes The modes to add.
 */
void Modes::addModes(unsigned int modes) { this->modes |= modes; }

/**
 * @brief Removes one or more modes from the user.
 *
 * @param modes The modes to remove.
 */
void Modes::removeModes(unsigned int modes) { this->modes &= ~modes; }

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
 * @warning This function will return an empty string if the user is not a channel operator or voice. And please note
 * that this is mainly used for channel prefixes.
 * @return string
 */
string Modes::getModesString() const {
	string result;
	if (hasModes(M_OPERATOR)) {
		result += "@";
	} else if (hasModes(M_VOICE)) {
		result += "+";
	}
	return result;
}

/**
 * @brief Clears all modes.
 */
void Modes::clearModes() { modes = 0; }
