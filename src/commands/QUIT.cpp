#include "Codes.hpp"
#include "Exceptions.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

void QUIT(IRStream &stream, string &args, User *user) {
	(void)stream;
	(void)user;

	string reason = args.empty() ? "Client quit" : args.substr(1);
	throw UserQuitException(reason);
}
