#include <iostream>

#include "../include/Server.hpp"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: ./ircserver [port] [password]" << std::endl;
		return 1;
	}

	Server server(argv[2], argv[1]);

	return 0;
}
