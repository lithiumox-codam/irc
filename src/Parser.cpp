#include <iostream>
#include <stdexcept>
#include <string>

#include "General.hpp"
#include "IRStream.hpp"
#include "Codes.hpp"

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
		cout << "DEBUG: Line recv: " << buffer << "\n";
		if (buffer.empty()) {
			return true;
		}

		vector<string> commands = split(buffer, "\r\n");
		for (const string &command : commands) {
			if (command.empty()) {
				continue;
			}

			bool found = false;

			string baseCommand = command.substr(0, command.find(' '));

			for (const auto &pair : store) {
				if (baseCommand == pair.first) {
					found = true;

					stream.setCommand(baseCommand);

					string args;
					if (command.size() > baseCommand.size()) {
						args = command.substr(baseCommand.size() + 1);
					}

					if (!pair.second(stream, args, user)) {
						stream.sendPacket(user); // why?
						return false;
					}
				}
			}

			if (!found) {
				stream.prefix()
				.code(ERR_UNKNOWNCOMMAND)
				.param(user->getNickname()) // if nick is not yet set, might have an issue
				.param(baseCommand)
				.trail("Unknown Command")
				.end();
			}
		}
	} catch (const runtime_error &e) {
		return false;
	}
	stream.sendPacket(user); // why?
	return true;
}
