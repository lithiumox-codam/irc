#pragma once

#include <string>
#include <unordered_map>

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
ostream &operator<<(ostream &os, const PacketType &type);

std::unordered_map<PacketType, string> parse(const string &message);
