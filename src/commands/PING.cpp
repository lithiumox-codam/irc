#include <vector>

#include "General.hpp"

bool PING(stringstream &stream, string &args, User &user) {
	(void)user;
	if (args.empty()) {
		stream << startRes(ERR_NOORIGIN) << " " << user.getNickname() << " :No origin specified" << END;
		return false;
	}
	const vector<string> tokens = split(args, ' ');
	stream << "PONG :" << args << END;
	return true;
}
