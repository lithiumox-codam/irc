#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <string>

#include "Channel.hpp"
#include "User.hpp"

using namespace std;

// NOLINTNEXTLINE
enum class ServerConfig {
	BACKLOG = 10,
	MAX_EVENTS = 10,
	BUFFER_SIZE = 1024,
};

class Server {
   private:
	vector<Channel> channels;  // List of channels
	vector<User> users;		   // List of users

	string password;  // Password for connecting to the server
	string hostname;  // Hostname of the server

	int socket;	   // Socket file descriptor
	int port;	   // Port number
	bool running;  // Server running status

	int epoll_fd;  // File descriptor for epoll

   public:
	Server();
	~Server();

	// Getters and Setters
	void setPassword(const string &password);
	[[nodiscard]] auto getPassword() const -> const string &;

	// Public Methods
	void bindSocket(const string &portString);

	void start();
	void stop();

	void epollCreate();
	void epollAdd(int socket);
	void epollRemove(int socket);
	void epollChange(int socket, uint32_t events);
	void epollEvent(struct epoll_event &event);
	void epollWait();

	void addUser(unsigned int socket);
	void removeUser(User &user);
	[[nodiscard]] auto getUser(int socket) -> User &;
	[[nodiscard]] auto getUsers() const -> const vector<User> &;

	auto addChannel(string &channelName) -> Channel &;
	void removeChannel(Channel &channel);
	[[nodiscard]] auto getChannels() -> vector<Channel> &;
	[[nodiscard]] auto getChannel(const string &name) -> Channel &;

   private:
};
