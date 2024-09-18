#pragma once

#include <sstream>
#include <string>
#include <map>

#include "User.hpp"

using namespace std;

/*
 * Color related definitions. (Pastels)
 */
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define ITALIC "\033[3m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"
#define INVERT "\033[7m"
#define STRIKETHROUGH "\033[9m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

/* A struct that maps a key to a PacketType also used by the PacketProcessor later on. */
using PacketTypeMap = struct {
	const string key;
	void (*func)(string&, User&);
};

bool CAP(std::stringstream& stream, string& args, User& user);
bool NICK(std::stringstream& stream, string& args, User& user);
bool USER(std::stringstream& stream, string& args, User& user);
bool PASS(std::stringstream& stream, string& args, User& user);
bool INFO(std::stringstream& stream, string& args, User& user);
bool JOIN(std::stringstream& stream, string& args, User& user);
bool PING(std::stringstream& stream, string& args, User& user);

/**
 * @brief The store array is a map of PacketType and the key to look for in a message.
 * @note If you add a new PacketType, make sure to add it to the store array. If you don't, the parse function will
 * not be able to find the key in the message.
 */
// have a function poitner to the function that will be called
const std::map<string, bool (*)(std::stringstream&, string&, User&)> store = {
	{"CAP", CAP}, {"NICK", NICK}, {"USER", USER}, {"PASS", PASS}, {"INFO", INFO}, {"JOIN", JOIN}, {"PING", PING}};

bool parse(User& user);
