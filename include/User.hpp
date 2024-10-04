#pragma once

#include <cstdint>
#include <string>

#include "Modes.hpp"

using namespace std;

enum UserConfig : uint16_t {
	BUFFER_SIZE = 1024	// The size of the buffer that will be used to read from the socket.
};

unsigned int const U_INFO = 1 << 1;
unsigned int const U_USER = 1 << 2;
unsigned int const U_NICK = 1 << 3;
unsigned int const U_AUTHENTICATED = 1 << 4;
unsigned int const U_WELCOME = 1 << 5;

/**
 * @brief Bitmask representing the completion status of a user.
 *
 * This constant is a combination of several user status flags:
 * - U_INFO: User information is available.
 * - U_USER: User is registered.
 * - U_NICK: User has a nickname.
 * - U_AUTHENTICATED: User is authenticated.
 * - U_WELCOME: User has received a welcome message.
 *
 * When all these flags are set, the user is considered to have completed all necessary steps.
 */
unsigned int const U_COMPLETED = U_INFO | U_USER | U_NICK | U_AUTHENTICATED | U_WELCOME;

class User {
   private:
	string username;
	string nickname;
	string realname;
	string hostname;

	int socket;
	unsigned int handshake;

	string in_buffer;
	string out_buffer;

   public:
	Modes modes;
	User(int socket);
	User(const User &user) noexcept;
	auto operator=(const User &user) noexcept -> User &;
	virtual ~User() = default;

	void closeSocket();
	[[nodiscard]] int getSocket() const;

	[[nodiscard]] const string &getNickname() const;
	[[nodiscard]] const string &getUsername() const;
	[[nodiscard]] const string &getRealname() const;
	[[nodiscard]] const string &getHostname() const;

	void setNickname(string &nickname);
	void setUsername(string &username);
	void setRealname(string &realname);
	void setHostname(string &hostname);

	void addHandshake(unsigned int handshake);
	[[nodiscard]] bool hasHandshake(unsigned int handshake) const;
	[[nodiscard]] unsigned int getHandshake() const;

	[[nodiscard]] bool checkPacket();

	int readFromSocket();
	int sendToSocket();
	void addToBuffer(const string &data);

	[[nodiscard]] string &getInBuffer();
	[[nodiscard]] string &getOutBuffer();
	void clearInBuffer();
	void clearOutBuffer();
};

ostream &operator<<(ostream &stream, const User &user);
