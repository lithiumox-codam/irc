#pragma once

#include <inttypes.h>
#include <sys/epoll.h>
#include <array>

using namespace std;

#define MAXEVENTS 10

class EpollClass { 
	private:
		int		fd;

	public:
		EpollClass();
		EpollClass &operator=(const EpollClass &other);
		EpollClass(const EpollClass &other);
		~EpollClass();

		// Event list
		array<struct epoll_event, MAXEVENTS>		events;

		// Functions
		void			add(int socket_fd);
		void 			change(int socket_fd, uint32_t events);
		void			remove(int socket_fd);
		void			wait();
} ;
