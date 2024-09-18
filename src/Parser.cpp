#include <algorithm>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>

#include "General.hpp"

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
 * @return unordered_map<PacketType, string> A map of PacketType and the message representing the packet
 */
bool parse(User &user) {
	std::stringstream stream;
	try {
		string &buffer = user.getInBuffer();
		if (buffer.empty()) {
			return false;
		}
		for (const auto &pair : store) {
			size_t found = buffer.find(pair.first);
			if (found != string::npos) {
				string args = getArgs(buffer, found);
				if (!pair.second(stream, args, user)) {
					cout << stream.str();
					buffer = stream.str() + "\r\n";
					return false;
				}
				user.addToBuffer(stream.str() + "\r\n");
				cout << pair.first << stream.str() << "\n";
			}
		}
	} catch (const runtime_error &e) {
		return false;
	}
	return true;
}
