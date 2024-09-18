#include "General.hpp"

bool NICK(stringstream &stream, string &args, User &user) {
	if (args.empty()) {
		stream.flush();
		return false;
	}
	if (args.find(' ') != string::npos) {
		stream << "Error: NICK packet has a space in the argument" << "\n";
		return false;
	}
	user.setNickname(args);
	user.addHandshake(U_NICK);
	stream << ":" << user.getNickname() << " NICK " << user.getNickname() << END;
	return true;
}
