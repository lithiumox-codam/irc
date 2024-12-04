#pragma once

#include <cstdint>
#include <string>

#include "Modes.hpp"

using namespace std;

enum UserConfig : uint16_t {
	BUFFER_SIZE = 1024	// The size of the buffer that will be used to read from the socket.
};

/**
 * @brief Bitmask representing the completion status of a user.
 *
 * @details The Handshake enum is a bitmask that represents the completion status of a user. The flags are used to
 determine
 * if a user has completed all necessary steps to be considered registered.

 * - The H_INFO flag is set when the user has completed CAP negotiation.
 * - The H_USER flag is set when the user has user information.
 * - The H_NICK flag is set when the user has a nickname.
 * - The H_PASS flag is set when the user has entered the correct password.
 * - The H_WELCOME flag is set when the user has received a welcome message.
 *
 * - The H_AUTHENTICATED flag is set when the user has completed all necessary steps, except for sending the welcome
 message.
 * - The H_REGISTERED flag is set when the user has completed all necessary steps.
 */
enum Handshake : uint8_t {
	H_INFO = 1 << 0,	 // User has completed CAP negotiation.
	H_USER = 1 << 1,	 // User has user information.
	H_NICK = 1 << 2,	 // User has a nickname.
	H_PASS = 1 << 3,	 // User has entered the correct password.
	H_WELCOME = 1 << 4,	 // User has received a welcome message.

	H_AUTHENTICATED = H_USER | H_NICK | H_PASS |
					  H_INFO,  // User has completed all necessary steps, except for sending the welcome message.
	H_REGISTERED = H_AUTHENTICATED | H_WELCOME,	 // User has completed all necessary steps.
};

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

	[[nodiscard]] string getNickname() const;
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

	void readFromSocket();
	void sendToSocket();

	[[nodiscard]] string &getInBuffer();

	[[nodiscard]] string &getOutBuffer();
	void addToBuffer(const string &data);

	void parse(void);
};

ostream &operator<<(ostream &stream, const User &user);
