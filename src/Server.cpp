/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opelser <opelser@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 19:25:13 by opelser           #+#    #+#             */
/*   Updated: 2024/06/11 19:25:20 by opelser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

#include <iostream>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

// ************************************************************************** //
//                        Constructors and Destructors                        //
// ************************************************************************** //

Server::Server(void)
{
	std::cout << GREEN << "Server: Default constructor called" << RESET << std::endl;
}

Server::Server(const Server &rhs)
{
	std::cout << GREEN << "Server: Copy constructor called" << RESET << std::endl;

	*this = rhs;
}

Server &
Server::operator=(const Server &rhs)
{
	std::cout << GREEN << "Server: Assignment operator called" << RESET << std::endl;

	if (this != &rhs)
	{
		// Perform deep copy
	}

	return (*this);
}

Server::~Server(void)
{
	std::cout << RED << "Server: Destructor called" << RESET << std::endl;
}

// ************************************************************************** //
//                                Public methods                              //
// ************************************************************************** //


