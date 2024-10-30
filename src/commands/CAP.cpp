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
bool CAP(IRStream &stream, string &args, User *user) {
	if (args.empty()) {
		stream.prefix().param(user->getNickname()).param(" :Not enough parameters").end();
		return false;
	}

	vector<string> tokens = split(args, ' ');

	string command = tokens[0];
	tokens.erase(tokens.begin());

	if (command == "LS") {
		stream.prefix()
		.command()
		.param("*")
		.param("LS")
		.param(CAPABILITIES)
		.end();
	} else if (command == "REQ") {
		stream.prefix()
		.command()
		.param("*")
		.param("ACK")
		.params(tokens)
		.end();
	} else if (command == "END") {
		user->addHandshake(USER_INFO);

		if (user->hasHandshake(USER_AUTHENTICATED) &&
			!user->hasHandshake(USER_WELCOME)) {
			string empty;
			MOTD(stream, empty, user);
			user->addHandshake(USER_WELCOME);
		}
		return true;
	} else {
		stream.prefix()
		.code(ERR_UNKNOWNCOMMAND)
		.param(user->getNickname())
		.trail("CAP :Unknown command")
		.end();
		return false;
	}

	return true;
}
