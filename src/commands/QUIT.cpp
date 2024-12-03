#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "Exceptions.hpp"

extern Server server;

void QUIT(IRStream &stream, string &args, User *user) {
	(void) stream;
	(void) user;

	string reason = args.empty() ? "Client quit" : args.substr(1);
	throw UserQuitException(reason);
}
