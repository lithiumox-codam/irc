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
 * @details The UserFlags enum is a bitmask that represents the completion status of a user. The flags are used to determine
 * if a user has completed all necessary steps to be considered registered.

 * - The USER_INFO flag is set when the user has completed CAP negotiation.
 * - The USER_USER flag is set when the user has user information.
 * - The USER_NICK flag is set when the user has a nickname.
 * - The USER_PASS flag is set when the user has entered the correct password.
 * - The USER_WELCOME flag is set when the user has received a welcome message.
 *
 * - The USER_AUTHENTICATED flag is set when the user has completed all necessary steps, except for sending the welcome message.
 * - The USER_REGISTERED flag is set when the user has completed all necessary steps.
 */
enum UserFlags : uint8_t {
	USER_INFO			= 1 << 0,				// User has completed CAP negotiation.
	USER_USER			= 1 << 1,				// User has user information.
	USER_NICK			= 1 << 2,				// User has a nickname.
	USER_PASS 			= 1 << 3,				// User has entered the correct password.
	USER_WELCOME		= 1 << 4,				// User has received a welcome message.

	USER_AUTHENTICATED	= USER_USER | USER_NICK | USER_PASS | USER_INFO,	// User has completed all necessary steps, except for sending the welcome message.
	USER_REGISTERED		= USER_AUTHENTICATED | USER_WELCOME,				// User has completed all necessary steps.
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

	[[nodiscard]] string &getInBuffer();
	[[nodiscard]] string &getOutBuffer();

	// Socket I/O
	bool	readFromSocket();
	bool	sendToSocket();
	void	addToBuffer(const string &data);

	void	parse(void);

};

ostream &operator<<(ostream &stream, const User &user);
