#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <vector>

#include "General.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

static vector<string> split(const string &str, const char &delim) {
	vector<string> tokens;
	string token;
	for (const char &character : str) {
		if (character == delim) {
			tokens.push_back(token);
			token.clear();
		} else {
			token += character;
		}
	}
	tokens.push_back(token);
	return tokens;
}

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
bool CAP(std::stringstream &stream, string &args, User &user) {
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
		stream << ":localhost CAP * LS\r\n";
	} else if (command == "REQ") {
		stream << ":localhost CAP * NAK ";
		for (const string &token : tokens) {
			stream << token << " ";
		}
		stream << "\r\n";
	} else if (command == "END") {
		return true;
	} else {
		stream.flush();
		stream << ":localhost 421 " + user.getNickname() + " CAP :Unknown command\r\n";
		return false;
	}

	return true;
}

bool NICK(std::stringstream &stream, string &args, User &user) {
	stream << "User " << user.getNickname() << " has changed their nickname to " << args << "\n";
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
	stream << ":" << user.getNickname() << " NICK " << user.getNickname() << "\r\n";
	return true;
}

bool USER(std::stringstream &stream, string &args, User &user) {
	vector<string> tokens = split(args, ' ');
	if (tokens.size() < 4) {
		stream << "Error: USER packet has less than 4 arguments" << "\n";
		return false;
	}

	user.setUsername(tokens[0]);
	user.setRealname(tokens[3]);
	user.setHostname(tokens[2]);
	user.addHandshake(U_USER);

	// stream the response to the user this has to be valid irc response
	stream << ":" << user.getNickname() << " USER " << user.getUsername() << " " << user.getHostname() << " "
		   << server.getHostname() << " :" << user.getRealname() << "\r\n";
	return true;
}

bool PASS(std::stringstream &stream, string &args, User &user) {
	if (args.empty()) {
		stream << "Error: PASS packet has less than 1 argument" << "\n";
		return false;
	}
	if (args == server.getPassword()) {
		user.addHandshake(U_AUTHENTICATED);
	} else {
		stream << "Error: User has failed to authenticate expecte: \"" << server.getPassword() << "\" got: \"" << args
			   << "\"" << "\n";
	}
	user.addHandshake(U_WELCOME);
	return true;
}

bool INFO(std::stringstream &stream, string &args, User &user) {
	user.printUser();

	if (args.empty()) {
		stream << "Error: INFO packet has less than 1 argument" << "\n";
		return false;
	}

	return true;
}

bool JOIN(std::stringstream &stream, string &args, User &user) {
	if (args.empty()) {
		cerr << "Error: JOIN packet has less than 1 argument" << "\n";
		return false;
	}
	stream << ":" << user.getNickname() << " JOIN " << args << "\r\n";
	return true;
}

bool PING(std::stringstream &stream, string &args, User &user) {
	(void)user;
	if (args.empty()) {
		cerr << "Error: PING packet has less than 1 argument"
			 << "\n";
		return false;
	}
	stream << "PONG :" << args << "\r\n";
	return true;
}
