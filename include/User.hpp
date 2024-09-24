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
	// User(User &user) = delete;
	// auto operator=(User &user) -> User & = delete;
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

	void printHandshake() const;
	void printUser() const;

	[[nodiscard]] bool checkPacket();

	int readFromSocket();
	int sendToSocket();
	void addToBuffer(const string &data);

	[[nodiscard]] string &getInBuffer();
	[[nodiscard]] string &getOutBuffer();
	void clearInBuffer();
	void clearOutBuffer();

	[[nodiscard]] static string getNextCommand(string &buffer);
};

auto operator<<(ostream &os, const User &user) -> ostream &;
