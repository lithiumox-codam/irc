#include <iostream>
#include <stdexcept>
#include <string>

#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"

static vector<string> getCommands(string &buffer, const string &delim) {
	vector<string> commands;
	size_t delimPos = 0;

	while (!buffer.empty()) {
		delimPos = buffer.find(delim);
		if (delimPos == string::npos) {
			break;
		}
		commands.push_back(buffer.substr(0, delimPos));
		buffer.erase(0, delimPos + delim.size());
	}
	return commands;
}

/**
 * @brief The parse function takes a message and returns a map of PacketType and the message.
 *
 * @param message The entire message to parse
 * @return bool Returns true if the message was parsed successfully, false when an error occurs during one of the called
 * functions.
 */
void User::parse(void) {
	IRStream stream;

	vector<string> commands = getCommands(this->getInBuffer(), "\r\n");
	for (const string &command : commands) {
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

				pair.second(stream, args, this);
			}
		}

		if (!found) {
			stream.prefix()
				.code(ERR_UNKNOWNCOMMAND)
				.param(this->getNickname())
				.param(baseCommand)
				.trail("Unknown Command")
				.end();
		}
	}

	stream.sendPacket(this);
}
