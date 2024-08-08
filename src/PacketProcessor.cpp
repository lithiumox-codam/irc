#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <vector>

#include "General.hpp"
#include "Server.hpp"
#include "User.hpp"

extern Server server;

static auto split(const string &str, const char &delim) -> vector<string> {
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
 * !TODO: instead of getting a user here we should get a user that is associated with the user and then do all the
 * processing from there. Maybe even the ChannelMember class while we are at it.
 */

void CAP(string &args, User &user) {
	if (args.empty()) {
		cerr << "Error: CAP command is invalid"
			 << "\n";
		return;
	}
	try {
		user.addHandshake(U_INFO);
		user.printUser();
	} catch (const runtime_error &e) {
		cerr << "Error: " << e.what() << "\n";
	}
}

void NICK(string &args, User &user) {
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
	user.printUser();
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
	user.printUser();

	cout << "User " << tokens[0] << " has connected"
		 << "\n";
}

void PASS(string &args, User &user) {
	if (args.empty()) {
		cerr << "Error: PASS packet has less than 1 argument"
			 << "\n";
		return;
	}
	if (args == server.getPassword()) {
		try {
			user.addHandshake(U_AUTHENTICATED);
			user.printUser();
		} catch (const runtime_error &e) {
			cerr << "Error: " << e.what() << "\n";
		}

	} else {
		cerr << "Error: User has failed to authenticate expecte: \"" << server.getPassword() << "\" got: \"" << args
			 << "\""
			 << "\n";
	}
}

void INFO(string &args, User &user) {
	user.printUser();

	if (args.empty()) {
		cerr << "Error: INFO packet has less than 1 argument"
			 << "\n";
		return;
	}
}

void JOIN(string &args, User &user) {
	user.printUser();

	if (args.empty()) {
		cerr << "Error: JOIN packet has less than 1 argument"
			 << "\n";
		return;
	}
}
