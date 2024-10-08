#include "Modes.hpp"

Modes::Modes() : modes(0) {}

Modes::Modes(unsigned int modes) : modes(modes) {}

Modes::Modes(const Modes &modes) noexcept : modes(modes.modes) {}

auto Modes::operator=(const Modes &modes) noexcept -> Modes & {
	this->modes = modes.modes;
	return *this;
}

void Modes::setModes(unsigned int modes) { this->modes = modes; }

unsigned int Modes::getModes() const { return modes; }

void Modes::addModes(unsigned int modes) { this->modes |= modes; }

void Modes::removeModes(unsigned int modes) { this->modes &= ~modes; }

bool Modes::hasModes(unsigned int modes) const { return (this->modes & modes) == modes; }

string Modes::printModes() const {
	string result;
	if (hasModes(M_OPERATOR)) {
		result += "o";
	}
	if (hasModes(M_VOICE)) {
		result += "v";
	}
	if (hasModes(M_INVISIBLE)) {
		result += "i";
	}
	return result;
}

void Modes::clearModes() { modes = 0; }
