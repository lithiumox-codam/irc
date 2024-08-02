/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opelser <opelser@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 19:17:58 by opelser           #+#    #+#             */
/*   Updated: 2024/08/02 18:21:13 by opelser          ###   ########.fr       */
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

using namespace std;

#define BUFFER_SIZE 1024

class Server {
   private:
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
};

#endif
