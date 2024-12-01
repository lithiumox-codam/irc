#pragma once

#include <map>
#include <string>
#include <vector>

#include "IRStream.hpp"
#include "User.hpp"

using namespace std;

/* Utils */
vector<string> split(const string& str, const char& delim);
vector<string> split(const string& str, const string& delim);
pair<string, string> splitPair(const string& str, const char& delim);
string join(const vector<string>& tokens, size_t start);

void CAP(IRStream& stream, string& args, User* user);
void NICK(IRStream& stream, string& args, User* user);
void USER(IRStream& stream, string& args, User* user);
void PASS(IRStream& stream, string& args, User* user);
void INFO(IRStream& stream, string& args, User* user);
void JOIN(IRStream& stream, string& args, User* user);
void PING(IRStream& stream, string& args, User* user);
void MOTD(IRStream& stream, string& args, User* user);
void PRIVMSG(IRStream& stream, string& args, User* user);
void WHO(IRStream& stream, string& args, User* user);
void TOPIC(IRStream& stream, string& args, User* user);
void PART(IRStream& stream, string& args, User* user);
void KICK(IRStream& stream, string& args, User* user);
void INVITE(IRStream& stream, string& args, User* user);
void MODE(IRStream& stream, string& args, User* user);
void LIST(IRStream& stream, string& args, User* user);

/**
 * @brief The store of all the commands. This is a map of strings to function pointers. Used by the parser to determine
 * which function to call.
 */
const map<string, void (*)(IRStream&, string&, User*)> store = {
	{"PASS", PASS}, {"CAP", CAP},		{"NICK", NICK},		  {"MOTD", MOTD}, {"USER", USER},	{"INFO", INFO},
	{"JOIN", JOIN}, {"PING", PING},		{"PRIVMSG", PRIVMSG}, {"WHO", WHO},	  {"TOPIC", TOPIC}, {"PART", PART},
	{"KICK", KICK}, {"INVITE", INVITE}, {"MODE", MODE},		  {"LIST", LIST}};

void parse(User* user);
