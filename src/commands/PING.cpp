#include "General.hpp"

bool PING(stringstream &stream, string &args, User &user) {
	(void)user;
	if (args.empty()) {
		stream << startRes(ERR_NOORIGIN) << " " << user.getNickname() << " :No origin specified" << END;
		return false;
	}
	stream << "PONG :" << args << END;
	return true;
}
