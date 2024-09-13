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
void CAP(string &args, User &user) {
	cout << PacketType::CAP << " " << args << "\n";

	if (args.empty()) {
		cerr << "Error: CAP command is invalid"
			 << "\n";
		return;
	}
	try {
		user.addHandshake(U_INFO);
	} catch (const runtime_error &e) {
		cerr << "Error: " << e.what() << "\n";
	}

	vector<string> tokens = split(args, ' ');

	string command = tokens[0];
	tokens.erase(tokens.begin());

	if (command == "LS") {
		user.addToBuffer(":localhost CAP * LS\r\n");
	} else if (command == "REQ") {
		user.addToBuffer(":localhost CAP * NAK ");
		for (const string &token : tokens) {
			user.addToBuffer(token + " ");
		}
		user.addToBuffer("\r\n");
	} else if (command == "END") {
		return;
	} else {
		user.addToBuffer(":localhost 421 " + user.getNickname() + " CAP :Unknown command\r\n");
	}
}

void NICK(string &args, User &user) {
	cout << PacketType::NICK << " " << args << "\n";

	if (args.empty()) {
		cerr << "Error: NICK packet has no argument"
			 << "\n";
		return;
	}
	if (args.find(' ') != string::npos) {
		cerr << "Error: NICK packet has a space in the argument"
			 << "\n";
		return;
	}
	user.setNickname(args);
	user.addHandshake(U_NICK);
	user.addToBuffer(":" + user.getNickname() + " NICK " + user.getNickname() + "\r\n");
}

void USER(string &args, User &user) {
	vector<string> tokens = split(args, ' ');
	if (tokens.size() < 4) {
		cerr << "Error: USER packet has less than 4 arguments"
			 << "\n";
		return;
	}

	user.setUsername(tokens[0]);
	user.setRealname(tokens[3]);
	user.setHostname(tokens[2]);
	user.addHandshake(U_USER);

	cout << "User " << tokens[0] << " has connected"
		 << "\n";
}

void PASS(string &args, User &user) {
	cout << PacketType::PASS << " " << args << "\n";

	if (args.empty()) {
		cerr << "Error: PASS packet has less than 1 argument"
			 << "\n";
		return;
	}
	if (args == server.getPassword()) {
		try {
			user.addHandshake(U_AUTHENTICATED);
		} catch (const runtime_error &e) {
			cerr << "Error: " << e.what() << "\n";
		}

	} else {
		cerr << "Error: User has failed to authenticate expecte: \"" << server.getPassword() << "\" got: \"" << args
			 << "\""
			 << "\n";
	}
	std::string welcomeMessage =
		":" + std::string("temp") + " 001 " + user.getNickname() + " :Welcome to the Internet Relay Network ";
	user.addToBuffer(welcomeMessage);
	user.addHandshake(U_WELCOME);
}

void INFO(string &args, User &user) {
	cout << PacketType::INFO << " " << args << "\n";

	user.printUser();

	if (args.empty()) {
		cerr << "Error: INFO packet has less than 1 argument"
			 << "\n";
		return;
	}
}

void JOIN(string &args, User &user) {
	cout << PacketType::JOIN << " " << args << "\n";

	if (args.empty()) {
		cerr << "Error: JOIN packet has less than 1 argument"
			 << "\n";
		return;
	}
	user.addToBuffer(":" + user.getNickname() + " JOIN " + args + "\r\n");
}

void PING(string &args, User &user) {
	cout << PacketType::PING << args << "\n";

	if (args.empty()) {
		cerr << "Error: PING packet has less than 1 argument"
			 << "\n";
		return;
	}
	user.addToBuffer("PONG :" + args + "\r\n");
}
