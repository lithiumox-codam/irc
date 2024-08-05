#include <iostream>
#include <ostream>
#include <string>

#include "General.hpp"

/**
 * @brief The PacketType enum is used to determine the type of packet.
 *
 * @note If you add a new PacketType, and want it to be fancy printed, make sure to add it to this list.
 */
ostream &operator<<(ostream &os, const PacketType &type) {
	switch (type) {
		case PacketType::CAP:
			os << BLUE << "CAP" << RESET;
			break;
		case PacketType::NICK:
			os << YELLOW << "NICK" << RESET;
			break;
		case PacketType::USER:
			os << CYAN << "USER" << RESET;
			break;
		case PacketType::PASS:
			os << GREEN << "PASS" << RESET;
			break;
		case PacketType::INFO:
			os << MAGENTA << "INFO" << RESET;
			break;
		case PacketType::JOIN:
			os << RED << "JOIN" << RESET;
			break;
		default:
			os << "Unknown packet type";
			break;
	}
	return os;
}

/**
 * @brief A super simple function to extract the parameters from a message.
 *
 * @param str The message to extract from
 * @param start Where to start extracting from
 * @return string The extracted string
 */
static string extract(const string &str, unsigned long start) {
	return str.substr(start, str.find("\r\n", start) - start);
}

/**
 * @brief The parse function takes a message and returns a map of PacketType and the message.
 *
 * @param message The entire message to parse
 * @return Packet A map of PacketType and the message representing the packet
 */
Packet parse(const string &message) {
	Packet parsed;

	for (const auto &item : store) {
		unsigned long pos = message.find(item.key);
		if (item.type != PacketType::NONE && pos != string::npos)
			parsed.insert({item.type, extract(message, (pos + item.key.length()) + 1)});
	}
	return parsed;
}
