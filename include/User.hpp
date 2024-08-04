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

class User {
   private:
	string username;
	string nickname;
	string realname;
	string hostname;

	unsigned int handshake;

   public:
	User(const string &username, const string &nickname);
	~User();
	const string &getUsername();
	const string &getNickname();
	void setNickname(const string &nickname);
	void addHandshake(unsigned int handshake);
	bool hasHandshake(unsigned int handshake);
	unsigned int getHandshake();
};
