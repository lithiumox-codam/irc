#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

bool LIST(IRStream &stream, string &args, User *user) {
	if (!user->hasHandshake(USER_REGISTERED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return false;
	}
	if (args.empty()) {
		stream.prefix().code(RPL_LISTSTART).param(user->getNickname()).trail("Channel :Users Name").end();
		for (auto &channel : server.getChannels()) {
			stream.prefix().code(RPL_LIST).param(user->getNickname()).param(channel.getName()).trail("0 :").end();
		}
		stream.prefix().code(RPL_LISTEND).param(user->getNickname()).trail("End of /LIST").end();
		return true;
	}
	return false;
}
