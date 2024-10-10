#pragma once

#include <map>
#include <string>
#include <vector>

#include "IRStream.hpp"
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

/* Utils */
vector<string> split(const string& str, const char& delim);

bool CAP(IRStream& stream, string& args, User* user);
bool NICK(IRStream& stream, string& args, User* user);
bool USER(IRStream& stream, string& args, User* user);
bool PASS(IRStream& stream, string& args, User* user);
bool INFO(IRStream& stream, string& args, User* user);
bool JOIN(IRStream& stream, string& args, User* user);
bool PING(IRStream& stream, string& args, User* user);
bool MOTD(IRStream& stream, string& args, User* user);
bool PRIVMSG(IRStream& stream, string& args, User* user);
bool WHO(IRStream& stream, string& args, User* user);
bool TOPIC(IRStream& stream, string& args, User* user);
bool PART(IRStream& stream, string& args, User* user);
bool MODE(IRStream& stream, string& args, User* user);
bool LIST(IRStream& stream, string& args, User* user);

/**
 * @brief The store of all the commands. This is a map of strings to function pointers. Used by the parser to determine
 * which function to call.
 */
const std::map<string, bool (*)(IRStream&, string&, User*)> store = {
	{"PASS", PASS},	  {"CAP", CAP},	  {"NICK", NICK}, {"MOTD", MOTD},		{"USER", USER},
	{"INFO", INFO},	  {"JOIN", JOIN}, {"PING", PING}, {"PRIVMSG", PRIVMSG}, {"WHO", WHO},
	{"TOPIC", TOPIC}, {"PART", PART}, {"MODE", MODE}, {"LIST", LIST}};

bool parse(User* user);
