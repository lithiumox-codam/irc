/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opelser <opelser@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 19:18:03 by opelser           #+#    #+#             */
/*   Updated: 2024/06/11 21:16:55 by opelser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void	receiveMessage(int client)
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

void	loop(int serv)
{
	while (1)
	{
		// Accept a connection
		const int		client = accept(serv, NULL, NULL);

		if (client == -1)
		{
			std::cerr << "Error: accept failed" << std::endl;
			return;
		}

		// Receive a message from the client
		receiveMessage(client);

		// Close the client socket
		close(client);
	}
}

int
main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " <port>" << std::endl;
		return (EXIT_FAILURE);
	}

	// Create a socket
	const int					serv = socket(AF_INET, SOCK_STREAM, 0);

	if (serv == -1)
	{
		std::cerr << "Error: socket creation failed" << std::endl;
		return (EXIT_FAILURE);
	}

	// Bind the socket to all addresses and the port
	const struct sockaddr_in	addr =
	{
		.sin_family = AF_INET,
		.sin_port = htons(std::atoi(av[1])),
		.sin_addr = {INADDR_ANY}
	};

	if (bind(serv, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		std::cerr << "Error: bind failed" << std::endl;
		return (EXIT_FAILURE);
	}

	// Listen for incoming connections, with a backlog of 10 pending connections
	if (listen(serv, 10) == -1)
	{
		std::cerr << "Error: listen failed" << std::endl;
		return (EXIT_FAILURE);
	}

	// Accept connections
	loop(serv);

	// Close the client socket
	close(serv);
	
	return (EXIT_SUCCESS);
}
