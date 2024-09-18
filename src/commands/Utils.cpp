#include <vector>

#include "General.hpp"
#include "Server.hpp"

extern Server server;

/**
 * @brief Splits a string by a delimiter.
 *
 * @param str The string to split.
 * @param delim The delimiter to split the string by.
 * @return vector<string> A vector of strings that have been split by the delimiter.
 */
vector<string> split(const string &str, const char &delim) {
	vector<string> tokens;
	string token;
	for (const char &character : str) {
		if (character == delim) {
			tokens.push_back(token);
			token.clear();
		} else {
			token += character;
		}
	}
	tokens.push_back(token);
	return tokens;
}

/**
 * @brief Generates the start of a response. Such as the server hostname and the response code.
 *
 * @param code The response code you want to send.
 * @return string The start of the response.
 *
 * @note The response code should be a 3 digit number. These are defined in include/Codes.hpp
 */
string startRes(const string &code) {
	string res;
	res.append(server.getHostname());
	res.append(code);
	res.append(" ");
	return res;
}
