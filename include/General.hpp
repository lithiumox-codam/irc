#pragma once

#include <array>
#include <string>
#include <unordered_map>

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

/*
 * Parser related definitions.
 */
// NOLINTNEXTLINE
enum class PacketType {
	NONE = -1,
	CAP,
	PASS,
	JOIN,
	PART,
	PRIVMSG,
	NOTICE,
	NICK,
	USER,
	QUIT,
	PING,
	PONG,
	MODE,
	TOPIC,
	INVITE,
	KICK,
	WHO,
	WHOIS,
	LIST,
	NAMES,
	MOTD,
	LUSERS,
	VERSION,
	STATS,
	LINKS,
	TIME,
	CONNECT,
	TRACE,
	ADMIN,
	INFO,
	SERVLIST,
};

auto operator<<(ostream &outputStream, const PacketType &type) -> ostream &;

/* A struct that maps a key to a PacketType also used by the PacketProcessor later on. */
using PacketTypeMap = struct {
	const string key;
	void (*func)(string &, User &);
};

void CAP(string &args, User &user);
void NICK(string &args, User &user);
void USER(string &args, User &user);
void PASS(string &args, User &user);
void INFO(string &args, User &user);
void JOIN(string &args, User &user);

/**
 * @brief The store array is a map of PacketType and the key to look for in a message.
 * @note If you add a new PacketType, make sure to add it to the store array. If you don't, the parse function will
 * not be able to find the key in the message.
 */

const std::array<PacketTypeMap, 7> store = {{
	{"CAP", &CAP},
	{"NICK", &NICK},
	{"USER", &USER},
	{"PASS", &PASS},
	{"INFO", &INFO},
	{"JOIN", &JOIN},
}};

void parse(User &user);
