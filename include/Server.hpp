/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opelser <opelser@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 19:17:58 by opelser           #+#    #+#             */
/*   Updated: 2024/06/11 19:25:02 by opelser          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>

class Server
{
	private:
		// Private Attributes
		int				_port;
		std::string		_password;
		

	public:
		// Constructors and Destructors
		Server(void);
		Server(const Server &rhs);
		Server &operator=(const Server &rhs);
		~Server(void);

		// Public Methods

} ;


#endif
