#include <utility>
#include <vector>

#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"

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
 * @brief Splits a string by a delimiter.
 *
 * @param str The string to split.
 * @param delim The delimiter to split the string by.
 * @return vector<string> A vector of strings that have been split by the delimiter.
 */
vector<string> split(const string &str, const string &delim) {
	vector<string> tokens;
	string token;

	for (size_t i = 0; i < str.size(); i++) {
		if (str.substr(i, delim.size()) == delim) {
			tokens.push_back(token);
			token.clear();
			i += delim.size() - 1;
		} else {
			token += str[i];
		}
	}

	tokens.push_back(token);
	return tokens;
}

pair<string, string> splitPair(const string &str, const char &delim) {
	size_t found = str.find(delim);
	if (found == string::npos) {
		return make_pair(str, "");
	}
	string first = str.substr(0, found);
	string second = str.substr(found + 1);

	if (!second.empty() && second[0] == ':') {
		second = second.substr(1);
	}
	return make_pair(first, second);
}

string join(const vector<string> &tokens, size_t start) {
	string result;
	for (size_t i = start; i < tokens.size(); ++i) {
		if (i > start) {
			result += " ";
		}
		result += tokens[i];
	}
	return result;
}
