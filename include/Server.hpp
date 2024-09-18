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

/* env related */
void getDotEnv();

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
	[[nodiscard]] const string &getPassword() const;

	// Public Methods
	void bindSocket(const string &portString);
	void setHostname(const string &hostString);

	void start();
	void stop();

	void epollCreate();
	void epollAdd(int socket);
	void epollRemove(int socket);
	void epollChange(int socket, uint32_t events);
	void epollEvent(struct epoll_event &event);
	int epollWait();
	void handleEvents(array<struct epoll_event, 10> &events, int numberOfEvents);
	void acceptNewConnection();

	void addUser(unsigned int socket);
	void removeUser(User &user);
	[[nodiscard]] User &getUser(int socket);
	[[nodiscard]] const vector<User> &getUsers() const;

	Channel &addChannel(string &channelName);
	void removeChannel(Channel &channel);
	[[nodiscard]] vector<Channel> &getChannels();
	[[nodiscard]] Channel &getChannel(const string &name);
	[[nodiscard]] const string &getHostname();

   private:
};
