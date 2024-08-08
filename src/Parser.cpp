#include <iostream>
#include <ostream>
#include <string>

#include "General.hpp"

/**
 * @brief The PacketType enum is used to determine the type of packet.
 *
 * @note If you add a new PacketType, and want it to be fancy printed, make sure to add it to this list.
 */
auto operator<<(ostream &outputStream, const PacketType &type) -> ostream & {
	switch (type) {
		case PacketType::CAP:
			outputStream << BLUE << "CAP" << RESET;
			break;
		case PacketType::NICK:
			outputStream << YELLOW << "NICK" << RESET;
			break;
		case PacketType::USER:
			outputStream << CYAN << "USER" << RESET;
			break;
		case PacketType::PASS:
			outputStream << GREEN << "PASS" << RESET;
			break;
		case PacketType::INFO:
			outputStream << MAGENTA << "INFO" << RESET;
			break;
		case PacketType::JOIN:
			outputStream << RED << "JOIN" << RESET;
			break;
		default:
			outputStream << "Unknown packet type";
			break;
	}
	return outputStream;
}

/**
 * @brief A super simple function to extract the parameters from a message.
 *
 * @param str The message to extract from
 * @param start Where to start extracting from
 * @return string The extracted string
//  */
// static auto extract(const string &str, unsigned long start) -> string {
// 	return str.substr(start, str.find("\r\n", start) - start);
// }

/**
 * @brief The parse function takes a message and returns a map of PacketType and the message.
 *
 * @param message The entire message to parse
 * @return unordered_map<PacketType, string> A map of PacketType and the message representing the packet
 */
void parse(User &user) {
	// extract the messages and call the packetProcessor functions directly
	// loop until the buffer is empty
	while (true) {
		try {
			string item = user.getNextCommand();
			for (const auto &packet : store) {
				if (item.find(packet.key) != string::npos) {
					packet.func(item, user);
				}
			}
		} catch (const runtime_error &e) {
			break;
		}
	}
	// unordered_map<PacketType, string> parsed;

	// for (const auto &item : store) {
	// 	unsigned long pos = message.find(item.key);
	// 	if (item.type != PacketType::NONE && pos != string::npos) {
	// 		parsed.insert({item.type, extract(message, (pos + item.key.length()) + 1)});
	// 	}
	// }
	// return parsed;
}
