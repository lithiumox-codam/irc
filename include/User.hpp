#pragma once

#include <string>

using namespace std;

/** Determines if the user has sent the INFO command. */
unsigned int const U_INFO = 1 << 1;
/** Determines if the user has sent the NICK and USER commands. */
unsigned int const U_REGISTERED = 1 << 2;
/** Determines if the user has sent the PASS command. */
unsigned int const U_AUTHENTICATED = 1 << 3;
/** Determines if the user has received the welcome message. */
unsigned int const U_WELCOME = 1 << 4;
/** Combines all the user flags. To easily check if someone is fully registered. */
unsigned int const U_COMPLETED = U_INFO | U_REGISTERED | U_AUTHENTICATED | U_WELCOME;

class User {
   private:
	string username;
	string nickname;
	string realname;
	string hostname;

	int socket;
	unsigned int handshake;

   public:
	User(const string &username, const string &hostname, const string &realname, int socket);
	~User();
	const string &getUsername();
	const string &getNickname();
	void setNickname(const string &nickname);
	void addHandshake(unsigned int handshake);
	bool hasHandshake(unsigned int handshake);
	unsigned int getHandshake();
};
