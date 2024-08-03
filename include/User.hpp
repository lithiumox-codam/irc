#pragma once

#include <string>

using namespace std;

/**
 * These are the possible handshake flags for a user.
 *
 * U_INFO: The user has sent the INFO command.
 * U_REGISTERED: The user has sent the NICK and USER commands.
 * U_AUTHENTICATED: The user has sent the PASS command.
 * U_WELCOME: The user has received the welcome message.
 */

unsigned int const U_INFO = 1 << 1;
unsigned int const U_REGISTERED = 1 << 2;
unsigned int const U_AUTHENTICATED = 1 << 3;
unsigned int const U_WELCOME = 1 << 4;

class User {
   private:
	string username;
	string nickname;

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
