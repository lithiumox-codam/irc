#include "Codes.hpp"
#include "General.hpp"

bool INFO(stringstream &stream, string &args, User &user) {
	user.printUser();

	if (args.empty()) {
		stream << startRes(ERR_NEEDMOREPARAMS) << user.getNickname() << " :Not enough parameters" << END;
		return false;
	}
	stream << startRes(RPL_INFO) << user.getNickname() << " :This is a test server" << END;
	return true;
}
