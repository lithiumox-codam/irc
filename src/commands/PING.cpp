#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"

bool PING(IRStream &stream, string &args, User *user) {
	(void)user;
	if (args.empty()) {
		stream.prefix().code(ERR_NOORIGIN).trail("No token specified").end();
	}

	stream.prefix().param("PONG").param(args).end();

	return true;
}
