#include <arpa/inet.h>

#include "General.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;
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
	user.addHandshake(U_INFO);

	vector<string> tokens = split(args, ' ');

	string command = tokens[0];
	tokens.erase(tokens.begin());

	if (command == "LS") {
		stream << "CAP * LS :multi-prefix" << END;
	} else if (command == "REQ") {
		stream << "CAP * ACK :";
		for (const string &token : tokens) {
			stream << token << " ";
		}
		stream << END;
	} else if (command == "END") {
		string empty;
		MOTD(stream, empty, user);
		return true;
	} else {
		stream.str("");
		stream << startRes(RPL_ISUPPORT) + user.getNickname() + " CAP :Unknown command" << END;
		return false;
	}

	return true;
}
