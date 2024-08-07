/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opelser <opelser@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 19:17:58 by opelser           #+#    #+#             */
/*   Updated: 2024/08/05 22:31:22 by opelser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <string>

#include "Channel.hpp"
#include "User.hpp"

using namespace std;

enum ServerConfig {
   BACKLOG = 10,
   MAX_EVENTS = 10,
   HOSTNAME_BUFFER_SIZE = 1024,
};

class Server {
   private:
	vector<Channel *> channels;  // List of channels
	vector<User *> users;	   // List of users

	string password;  // Password for connecting to the server
	string hostname;  // Hostname of the server

	int socket;	   // Socket file descriptor
	int port;	   // Port number
	bool running;  // Server running status

   public:
	Server();
	Server(const string &password, const string &port);
	Server(const Server &rhs);
	auto operator=(const Server &rhs) -> Server &;
	~Server();

	// Getters and Setters
	void setPassword(const string &password);
	[[nodiscard]] auto getPassword() const -> const string &;

	// Public Methods
	void bindSocket(const string &portString);
	void start();
	void stop();
	void sendMessage(int client, const string &message);
	void addUser(User *user);
	void removeUser(User *user);
	[[nodiscard]] auto getUsers() const -> const vector<User *> &;
};

#endif
