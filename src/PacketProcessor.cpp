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
 * !TODO: instead of getting a client here we should get a user that is associated with the client and then do all the
 * processing from there. Maybe even the ChannelMember class while we are at it.
 */

void CAP(string &args, const int &client) {
	if (args.empty()) {
		cerr << "Error: CAP command is invalid" << "\n";
		return;
	}
	try {
		User &user = server.getUser(client);
		user.addHandshake(U_INFO);
	} catch (const runtime_error &e) {
		cerr << "Error: " << e.what() << "\n";
	}
}

void NICK(string &args, const int &client) {
	if (args.empty()) {
		cerr << "Error: NICK packet has no argument" << "\n";
		return;
	}
	if (args.find(' ') != string::npos) {
		cerr << "Error: NICK packet has a space in the argument" << "\n";
		return;
	}
	try {
		User &user = server.getUser(client);
		user.setNickname(args);
		user.addHandshake(U_NICK);
	} catch (const runtime_error &e) {
		cerr << "Error: " << e.what() << "\n";
	}
}

void USER(string &args, const int &client) {
	(void)client;
	vector<string> tokens = split(args, ' ');
	if (tokens.size() < 4) {
		cerr << "Error: USER packet has less than 4 arguments" << "\n";
		return;
	}

	try {
		User &user = server.getUser(client);
		user.setUsername(tokens[0]);
		user.setRealname(tokens[3]);
		user.setHostname(tokens[2]);
		user.addHandshake(U_USER);
	} catch (const runtime_error &e) {
		cerr << "Error: " << e.what() << "\n";
	}

	cout << "User " << tokens[0] << " has connected" << "\n";
}

void PASS(string &args, const int &client) {
	if (args.empty()) {
		cerr << "Error: PASS packet has less than 1 argument" << "\n";
		return;
	}
	if (args == server.getPassword()) {
		try {
			User &user = server.getUser(client);
			user.addHandshake(U_AUTHENTICATED);
		} catch (const runtime_error &e) {
			cerr << "Error: " << e.what() << "\n";
		}

	} else {
		cerr << "Error: User has failed to authenticate expecte: \"" << server.getPassword() << "\" got: \"" << args
			 << "\"" << "\n";
	}
}

void INFO(string &args, const int &client) {
	(void)client;
	if (args.empty()) {
		cerr << "Error: INFO packet has less than 1 argument" << "\n";
		return;
	}
}

void JOIN(string &args, const int &client) {
	(void)client;

	if (args.empty()) {
		cerr << "Error: JOIN packet has less than 1 argument" << "\n";
		return;
	}
}

void packetProcessor(unordered_map<PacketType, string> &packet, const int &client) {
	for (auto &single : packet) {
		for (auto &singlePacket : store) {
			if (single.first == singlePacket.type) {
				singlePacket.func(single.second, client);
			}
		}
	}
}
