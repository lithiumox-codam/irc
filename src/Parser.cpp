#include <iostream>
#include <stdexcept>
#include <string>

#include "General.hpp"
#include "IRStream.hpp"

/**
 * @brief The parse function takes a message and returns a map of PacketType and the message.
 *
 * @param message The entire message to parse
 * @return bool Returns true if the message was parsed successfully, false when an error occurs during one of the called
 * functions.
 */
bool parse(User *user) {
	IRStream stream;
	try {
		string &buffer = user->getInBuffer();
		cout << "DEBUG: Buffer: " << buffer << "\n";
		if (buffer.empty()) {
			return true;
		}

		vector<string> commands = split(buffer, "\r\n");
		for (const string &command : commands) {
			if (command.empty()) {
				continue;
			}

			for (const auto &pair : store) {
				if (command.starts_with(pair.first)) {
					stream.setCommand(pair.first);
					string args = command.substr(pair.first.size() + 1);

					if (!pair.second(stream, args, user)) {
						stream.sendPacket(user); // why?
						return false;
					}
				}
			}
		}
	} catch (const runtime_error &e) {
		return false;
	}
	stream.sendPacket(user); // why?
	return true;
}
