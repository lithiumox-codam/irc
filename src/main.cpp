/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opelser <opelser@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 19:18:03 by opelser           #+#    #+#             */
/*   Updated: 2024/06/11 21:43:46 by opelser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include <iostream>

int
main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " <port>" << std::endl;
		return (EXIT_FAILURE);
	}

	Server		server;

	server.bindSocket(av[1]);
	server.start();
	server.stop();

	return (EXIT_SUCCESS);
}
