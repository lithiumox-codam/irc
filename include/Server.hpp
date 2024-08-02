/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opelser <opelser@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 19:17:58 by opelser           #+#    #+#             */
/*   Updated: 2024/06/11 21:43:09 by opelser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>

# define BUFFER_SIZE 1024

class Server
{
	private:
		// Private Attributes
		std::string		_password;	// Password for connecting to the server
		int				_socketfd;	// Socket file descriptor

	public:
		// Constructors and Destructors
		Server(std::string password, std::string port);
		Server(const Server &rhs);
		Server &operator=(const Server &rhs);
		~Server(void);

		// Getters and Setters
		void				setPassword(const std::string &password);
		const std::string	&getPassword(void) const;

		// Public Methods
		void				bindSocket(const std::string &port);
		void				start(void);
		void				stop(void);
} ;


#endif
