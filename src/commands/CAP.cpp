#include <arpa/inet.h>

#include <iostream>

#include "General.hpp"
#include "Server.hpp"
#include "User.hpp"

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

	CAP END: End the capability negotiation phase.
		client: CAP END
		server:

	CAP [UNKNOWN COMMAND]: Unknown command.
		server: 421 [NICK] CAP :Unknown command
*/
bool CAP(stringstream &stream, string &args, User &user) {
	if (args.empty()) {
		stream << "Error: CAP command is invalid" << "\n";
		return false;
	}
	try {
		user.addHandshake(U_INFO);
	} catch (const runtime_error &e) {
		stream << "Error: " << e.what() << "\n";
		return false;
	}

	vector<string> tokens = split(args, ' ');

	string command = tokens[0];
	tokens.erase(tokens.begin());

	if (command == "LS") {
		stream << "CAP * LS :multi-prefix sasl" << "\r\n";
	} else if (command == "REQ") {
		stream << ":localhost CAP * NAK " << "\r\n";
		for (const string &token : tokens) {
			stream << token << " ";
		}
		stream << "\r\n";
	} else if (command == "END") {
		return true;
	} else {
		stream.str("");
		stream << startRes(RPL_ISUPPORT) + user.getNickname() + " CAP :Unknown command" << "\r\n";
		return false;
	}

	return true;
}
