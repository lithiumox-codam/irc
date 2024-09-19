#include <iostream>
#include <ostream>
#include <string>

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
 * @return bool Returns true if the message was parsed successfully, false when an error occurs during one of the called
 * functions.
 */
bool parse(User &user) {
	static stringstream stream;
	try {
		string &buffer = user.getInBuffer();
		if (buffer.empty()) {
			return true;
		}
		cout << "Buffer: " << buffer << '\n';
		for (const auto &pair : store) {
			size_t found = buffer.find(pair.first);
			if (found != string::npos) {
				string args = getArgs(buffer, found);
				cout << "Command found: " << pair.first << " with args: " << args << '\n';
				if (!pair.second(stream, args, user)) {
					user.addToBuffer(stream.str());
					stream.str("");
					return false;
				}
			}
		}
	} catch (const runtime_error &e) {
		return false;
	}
	user.addToBuffer(stream.str());
	cout << "TEST: " << user.getOutBuffer() << '\n';
	stream.str("");
	return true;
}
