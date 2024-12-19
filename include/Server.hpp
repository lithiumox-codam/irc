#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <csignal>
#include <deque>
#include <string>

#include "Channel.hpp"
#include "User.hpp"

using namespace std;

enum class ServerConfig : std::uint16_t {
	BACKLOG = 10,
	MAX_EVENTS = 10,
	VECTOR_RESERVE = 10,
	BUFFER_SIZE = 1024,
};

typedef array<struct epoll_event, (size_t)ServerConfig::BACKLOG> EventArray;

class Server {
   private:
	deque<Channel> channels;   // List of channels
	deque<User> users;		   // List of users
	vector<string> operators;  // List of operators (specifically server operators)

	string password;  // Password for connecting to the server
	string hostname;  // Hostname of the server

	int socket;	   // Socket file descriptor
	int port;	   // Port number
	bool running;  // Server running status

	int epoll_fd;  // File descriptor for epoll

	void setPassword(string &password);
	void setPort(string &port);
	void setHostname(string &hostname);

	void socketCreate();
	void epollCreate();

   public:
	Server();
	~Server();

	// Getters and Setters
	[[nodiscard]] const string &getPassword() const;
	[[nodiscard]] const string &getHostname();
	[[nodiscard]] string getUserCount() const;

	// Public Methods
	void init(int argc, char **argv);
	void start();
	void stop();

	void acceptNewConnection();

	void epollAdd(int socket) const;
	void epollRemove(int socket) const;
	void epollChange(int socket, uint32_t events) const;
	int epollWait();
	void handleEvents(EventArray &events, int numberOfEvents);

	// User management
	void addUser(unsigned int socket);
	void removeUser(User &user, const string &reason);
	[[nodiscard]] User *getUser(int socket);
	[[nodiscard]] User *getUser(const string &nickname);
	[[nodiscard]] deque<User> &getUsers();

	// Channel management
	Channel &addChannel(const string &channelName);
	void removeChannel(Channel &channel);
	[[nodiscard]] Channel *getChannel(const string &name);
	[[nodiscard]] deque<Channel> &getChannels();

	// Server operator management
	void addOperator(const string &nickname);
	void removeOperator(const string &nickname);
	[[nodiscard]] bool operatorCheck(User *user) const;
	[[nodiscard]] bool operatorCheck(const string &nickname) const;
};

ostream &operator<<(ostream &stream, Server &server);
