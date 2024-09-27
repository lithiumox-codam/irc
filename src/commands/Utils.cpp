#include <vector>

#include "General.hpp"

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
