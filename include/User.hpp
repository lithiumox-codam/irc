#pragma once

#include <cstdint>
#include <string>

using namespace std;

enum UserConfig : uint16_t {
	BUFFER_SIZE = 1024	// The size of the buffer that will be used to read from the socket.
};
/** Determines if the user has sent the INFO command. */
unsigned int const U_INFO = 1 << 1;
/** Determines if the user has sent the USER command. */
unsigned int const U_USER = 1 << 2;
/** Determines if the user has sent the NICK command. */
unsigned int const U_NICK = 1 << 3;
/** Determines if the user has sent the PASS command. */
unsigned int const U_AUTHENTICATED = 1 << 4;
/** Determines if the user has received the welcome message. */
unsigned int const U_WELCOME = 1 << 5;
/** Combines all the user flags. To easily check if someone is fully registered. */
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
	User(int socket);
	User(User &user) = delete;
	auto operator=(User &user) -> User & = delete;
	User(User &&user) noexcept;
	auto operator=(User &&user) noexcept -> User &;
	virtual ~User();

	[[nodiscard]] auto getSocket() const -> int;

	[[nodiscard]] auto getUsername() const -> const string &;
	[[nodiscard]] auto getNickname() const -> const string &;
	void setNickname(string &nickname);
	void setUsername(string &username);
	void setRealname(string &realname);
	void setHostname(string &hostname);

	void addHandshake(unsigned int handshake);
	[[nodiscard]] auto hasHandshake(unsigned int handshake) const -> bool;
	[[nodiscard]] auto getHandshake() const -> unsigned int;

	void printHandshake() const;
	void printUser() const;

	[[nodiscard]] auto checkPacket() -> bool;

	auto readFromSocket() -> int;
	void addToBuffer(const string &data);
	auto getNextCommand() -> string;
};
