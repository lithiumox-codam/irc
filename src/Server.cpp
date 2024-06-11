/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opelser <opelser@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 19:25:13 by opelser           #+#    #+#             */
/*   Updated: 2024/06/11 21:55:20 by opelser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

// ************************************************************************** //
//                        Constructors and Destructors                        //
// ************************************************************************** //

Server::Server(void)
{
}

Server::Server(const Server &rhs)
{
	*this = rhs;
}

Server &
Server::operator=(const Server &rhs)
{
	if (this != &rhs)
	{
		this->_password = rhs._password;
		this->_socketfd = rhs._socketfd;
	}
	return (*this);
}

Server::~Server(void)
{
}

// ************************************************************************** //
//                            Getters and Setters                             //
// ************************************************************************** //

void
Server::setPassword(const std::string &password)
{
	this->_password = password;
}

const std::string &
Server::getPassword(void) const
{
	return (this->_password);
}

// ************************************************************************** //
//                                Public methods                              //
// ************************************************************************** //

void
Server::bindSocket(const std::string &portString)
{
	// Set the port
	in_port_t	port = htons(std::stoi(portString));

	// Create a socket
	this->_socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_socketfd == -1)
	{
		std::cerr << RED << "Error: socket creation failed" << RESET << std::endl;
		return;
	}

	// Bind the socket to all addresses and the port
	const struct sockaddr_in	addr =
	{
		.sin_family = AF_INET,
		.sin_port = port,
		.sin_addr = {INADDR_ANY}
	};

	if (bind(this->_socketfd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		std::cerr << RED << "Error: bind failed" << RESET << std::endl;
		return;
	}
}

static void
receiveMessage(int client)
{
	char	buffer[BUFFER_SIZE + 1];

	std::cout << "Received message:" << std::endl;

	while (recv(client, buffer, BUFFER_SIZE, 0) > 0)
	{
		buffer[BUFFER_SIZE] = '\0';
		std::cout << buffer;
	}
	std::cout << std::endl;
}

void
Server::start(void)
{
	// Listen for incoming connections, with a backlog of 10 pending connections
	if (listen(this->_socketfd, 10) == -1)
	{
		std::cerr << RED << "Error: listen failed" << RESET << std::endl;
		return;
	}
	else
	{
		std::cout << GREEN << "Server started" << std::endl;
	std::cout << "Press Ctrl+C to stop the server" << std::endl;
	std::cout << "Password: " << this->_password << RESET << std::endl;
	std::cout << "\nListening for incoming connections..." << std::endl;
	}

	// Listen for incoming connections, with a backlog of 10 pending connections
	while (1)
	{
		// Accept a connection
		const int		client = accept(this->_socketfd, NULL, NULL);

		if (client == -1)
		{
			std::cerr << RED << "Error: accept failed" << RESET << std::endl;
			exit (EXIT_FAILURE);
		}
		else
		{
			std::cout << GREEN << "Connection accepted" << RESET << std::endl;
		}

		// Receive a message from the client
		receiveMessage(client);

		// Close the client socket
		close(client);
	}
}

void
Server::stop(void)
{
	close(this->_socketfd);
}
