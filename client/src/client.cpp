/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opelser <opelser@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 21:07:34 by opelser           #+#    #+#             */
/*   Updated: 2024/06/11 21:10:33 by opelser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int
main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " <port>" << std::endl;
		return (EXIT_FAILURE);
	}

	// Create a socket
	const int					client = socket(AF_INET, SOCK_STREAM, 0);

	if (client == -1)
	{
		std::cerr << "Error: socket creation failed" << std::endl;
		return (EXIT_FAILURE);
	}

	// Connect to the server
	const struct sockaddr_in	addr =
	{
		.sin_family = AF_INET,
		.sin_port = htons(std::atoi(av[1])),
		.sin_addr = {INADDR_ANY}
	};

	if (connect(client, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
		std::cerr << "Error: connection failed" << std::endl;
		return (EXIT_FAILURE);
	}

	// Send a message to the server
	const char					*message = "Hello, server!";
	const int					messageLength = std::strlen(message);
	
	if (send(client, message, messageLength, 0) == -1)
	{
		std::cerr << "Error: message sending failed" << std::endl;
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
