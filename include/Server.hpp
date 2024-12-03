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

/* env related */
bool getEnv();

enum class ServerConfig : std::uint16_t {
	BACKLOG = 10,
	MAX_EVENTS = 10,
	VECTOR_RESERVE = 10,
	BUFFER_SIZE = 1024,
};

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

   public:
	Server();
	~Server();

	// Getters and Setters
	void setPassword(const string &password);
	[[nodiscard]] const string &getPassword() const;

	// Public Methods
	void bindSocket(const string &portString);
	bool isBound() const;
	void setHostname(const string &hostString);

	void start();
	void stop();

	void epollCreate();
	void epollAdd(int socket) const;
	void epollRemove(int socket) const;
	void epollChange(int socket, uint32_t events) const;
	int epollWait();

	void handleEvents(array<struct epoll_event, (size_t)ServerConfig::BACKLOG> &events, int numberOfEvents);
	void acceptNewConnection();

	void addUser(unsigned int socket);
	void removeUser(User &user, const string &reason);
	[[nodiscard]] User *getUser(int socket);
	[[nodiscard]] User *getUser(const string &nickname);
	[[nodiscard]] deque<User> &getUsers();

	Channel &addChannel(const string &channelName);
	void removeChannel(Channel &channel);
	[[nodiscard]] Channel *getChannel(const string &name);
	[[nodiscard]] deque<Channel> &getChannels();

	[[nodiscard]] const string &getHostname();
	void addOperator(const string &nickname);
	void removeOperator(const string &nickname);
	[[nodiscard]] bool operatorCheck(User *user) const;

	string getUserCount() const;
};

ostream &operator<<(ostream &stream, Server &server);
