#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "General.hpp"

ostream &operator<<(ostream &os, const PacketType &type) {
	switch (type) {
		case PacketType::CAP:
			os << BLUE << "CAP" << RESET;
			break;
		case PacketType::NICK:
			os << YELLOW << "NICK" << RESET;
			break;
		case PacketType::USER:
			os << CYAN << "USER" << RESET;
			break;
		case PacketType::PASS:
			os << GREEN << "PASS" << RESET;
			break;
		case PacketType::INFO:
			os << MAGENTA << "INFO" << RESET;
			break;
		case PacketType::JOIN:
			os << RED << "JOIN" << RESET;
			break;
		default:
			os << "Unknown packet type";
			break;
	}
	return os;
}

typedef struct func_arr {
	const string key;
	PacketType type;
	pair<PacketType, string> (*func)(PacketType, const string &, unsigned long);
} func_arr;

static pair<PacketType, string> extract(PacketType type, const string &str, unsigned long start) {
	if (type == PacketType::PASS) cout << str << endl;
	return {type, str.substr(start, str.find("\n") - 1)};
}

static const func_arr funcs[] = {{"CAP", PacketType::CAP, &extract},   {"NICK", PacketType::NICK, &extract},
								 {"USER", PacketType::USER, &extract}, {"PASS", PacketType::PASS, &extract},
								 {"INFO", PacketType::INFO, &extract}, {"JOIN", PacketType::JOIN, &extract},
								 {"", PacketType::NONE, nullptr}};

vector<string> split(const string &str, const string &delimiter) {
	vector<string> parts;
	size_t start = 0;
	size_t end = 0;

	while ((end = str.find(delimiter, start)) != string::npos) {
		parts.push_back(str.substr(start, (end - start)));
		start = end + delimiter.length();
	}
	if (start != str.length()) parts.push_back(str.substr(start));
	return parts;
}

unordered_map<PacketType, string> parse(const string &message) {
	unordered_map<PacketType, string> parsed;
	for (const auto &func : funcs) {
		unsigned long pos = message.find(func.key);
		if (func.func && pos != string::npos) {
			pair<PacketType, string> result = func.func(func.type, message, (pos + func.key.length()) + 1);
			parsed.insert(result);
		}
	}
	return parsed;
}
