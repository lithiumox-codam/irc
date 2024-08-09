#include <iostream>
#include <ostream>
#include <string>

#include "General.hpp"

#include <algorithm>

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
		case PacketType::PING:
			outputStream << RED << "PING" << RESET;
			break;
		default:
			outputStream << "Unknown packet type";
			break;
	}
	return outputStream;
}

static void trim(string &str) {
	str.erase(0, str.find_first_not_of(" \t\r\n"));
	str.erase(str.find_last_not_of(" \t\r\n") + 1);
}

/**
 * @brief The parse function takes a message and returns a map of PacketType and the message.
 *
 * @param message The entire message to parse
 * @return unordered_map<PacketType, string> A map of PacketType and the message representing the packet
 */
void parse(User &user) {
	try {
		string command = user.getNextCommand(user.getInBuffer());
		trim(command);

		string key = command.substr(0, command.find(' '));
		string arguments = command.substr(command.find(' ') + 1);

		cout << "Command: " << command << '\n';

		auto iter = std::find_if(store.begin(), store.end(), \
		[&key](const auto &packet) {
			return packet.key == key;
		});
		if (iter == store.end()) {
			// unknown command
			return;
		}
		iter->func(arguments, user);
	} catch (const runtime_error &e) {
		return;
	}
}
