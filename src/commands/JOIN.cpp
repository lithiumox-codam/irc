#include "General.hpp"
#include "Server.hpp"

extern Server server;

bool JOIN(stringstream &stream, string &args, User &user) {
	if (args.empty()) {
		stream.str("");
		stream << startRes(ERR_NEEDMOREPARAMS) << user.getNickname() << " :Not enough parameters" << END;
		return false;
	}

	stream << ":" << user.getNickname() << " JOIN " << args << END;

	return true;
}
