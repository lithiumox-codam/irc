#include <arpa/inet.h>

#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

#define CAPABILITIES ":multi-prefix sasl"

/*
	CAP LS: List supported capabilities.
		client: CAP LS [VERSION]
		server: CAP * LS :[CAPABILITIES]

	CAP REQ: Request to enable specific capabilities.
		client: CAP REQ :[CAPABILITIES]
		server: CAP * ACK :[CAPABILITIES]

	CAP ACK/NAK: Acknowledge or deny the capability request.
		server: CAP * ACK :[CAPABILITIES]
		server: CAP * NAK :[CAPABILITIES]

	CAP "\r\n": End the capability negotiation phase.
		client: CAP "\r\n"
		server:

	CAP [UNKNOWN COMMAND]: Unknown command.
		server: 421 [NICK] CAP :Unknown command
*/
void CAP(IRStream &stream, string &args, User *user) {
	string nickname = user->getNickname().empty() ? "*" : user->getNickname();
	if (args.empty()) {
		stream.prefix().param(nickname).param(" :Not enough parameters").end();
		return;
	}

	vector<string> tokens = split(args, ' ');

	string command = tokens[0];
	tokens.erase(tokens.begin());

	if (command == "LS") {
		stream.prefix().command().param("*").param("LS").param(CAPABILITIES).end();
	} else if (command == "REQ") {
		stream.prefix().command().param("*").param("ACK").params(tokens).end();
	} else if (command == "END") {
		user->addHandshake(H_INFO);

		if (user->hasHandshake(H_AUTHENTICATED) && !user->hasHandshake(H_WELCOME)) {
			string empty;
			MOTD(stream, empty, user);
			user->addHandshake(H_WELCOME);
		}
	} else {
		stream.prefix().code(ERR_UNKNOWNCOMMAND).param(nickname).trail("CAP :Unknown command").end();
	}
}
