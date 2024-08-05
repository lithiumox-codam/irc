/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: opelser <opelser@student.42.fr>              +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/11 19:17:58 by opelser       #+#    #+#                 */
/*   Updated: 2024/08/05 11:37:34 by mdekker       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <iostream>
#include <string>

#include "Channel.hpp"
#include "User.hpp"

using namespace std;

#define BUFFER_SIZE 1024
#define SERVER_NAME "ft_irc"

class Server {
   private:
	vector<Channel> channels;  // List of channels
	vector<User> users;		   // List of users

	string password;  // Password for connecting to the server
	string hostname;  // Hostname of the server

	int socket;	   // Socket file descriptor
	int port;	   // Port number
	bool running;  // Server running status

   public:
	Server();
	Server(const string &password, const string &port);
	Server(const Server &rhs);
	Server &operator=(const Server &rhs);
	~Server(void);

	// Getters and Setters
	void setPassword(const string &password);
	const string &getPassword(void) const;

	// Public Methods
	void bindSocket(const string &portString);
	void start(void);
	void stop(void);
	void sendMessage(int client, const string &message);
	void addUser(const User &user);
	void removeUser(User &user);
};

#endif
