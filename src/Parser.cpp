#include <iostream>
#include <stdexcept>
#include <string>

#include "General.hpp"
#include "IRStream.hpp"

static string getArgs(string &buffer, size_t found) {
	size_t start = buffer.find(' ', found);
	if (start == string::npos) {
		return "";
	}
	start++;
	size_t end = buffer.find("\r\n", start);
	if (end == string::npos) {
		return "";
	}

	return buffer.substr(start, end - start);
}

/**
 * @brief The parse function takes a message and returns a map of PacketType and the message.
 *
 * @param message The entire message to parse
 * @return bool Returns true if the message was parsed successfully, false when an error occurs during one of the called
 * functions.
 */
bool parse(User &user) {
	IRStream stream;
	try {
		string &buffer = user.getInBuffer();
		cout << "Buffer: " << buffer << endl;
		if (buffer.empty()) {
			return true;
		}
		for (const auto &pair : store) {
			size_t found = buffer.find(pair.first);
			if (found != string::npos) {
				stream.setCommand(pair.first);
				string args = getArgs(buffer, found);
				if (!pair.second(stream, args, user)) {
					stream.sendPacket(user);
					return false;
				}
			}
		}
	} catch (const runtime_error &e) {
		return false;
	}
	stream.sendPacket(user);
	return true;
}
