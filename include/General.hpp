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
vector<string> split(const string &str, const char &delim);
vector<string> split(const string &str, const string &delim);
pair<string, string> splitPair(const string &str, const char &delim);

void CAP(IRStream &stream, const string &args, User *user);
void NICK(IRStream &stream, const string &args, User *user);
void USER(IRStream &stream, const string &args, User *user);
void PASS(IRStream &stream, const string &args, User *user);
void INFO(IRStream &stream, const string &args, User *user);
void JOIN(IRStream &stream, const string &args, User *user);
void PING(IRStream &stream, const string &args, User *user);
void MOTD(IRStream &stream, const string &args, User *user);
void PRIVMSG(IRStream &stream, const string &args, User *user);
void WHO(IRStream &stream, const string &args, User *user);
void TOPIC(IRStream &stream, const string &args, User *user);
void PART(IRStream &stream, const string &args, User *user);
void KICK(IRStream &stream, const string &args, User *user);
void INVITE(IRStream &stream, const string &args, User *user);
void MODE(IRStream &stream, const string &args, User *user);
void LIST(IRStream &stream, const string &args, User *user);

/**
 * @brief The store of all the commands. This is a map of strings to function pointers. Used by the parser to determine
 * which function to call.
 */
const map<string, void (*)(IRStream &, const string &, User *)> store = {
	{"PASS", PASS}, {"CAP", CAP},		{"NICK", NICK},		  {"MOTD", MOTD}, {"USER", USER},	{"INFO", INFO},
	{"JOIN", JOIN}, {"PING", PING},		{"PRIVMSG", PRIVMSG}, {"WHO", WHO},	  {"TOPIC", TOPIC}, {"PART", PART},
	{"KICK", KICK}, {"INVITE", INVITE}, {"MODE", MODE},		  {"LIST", LIST}};

void parse(User *user);
