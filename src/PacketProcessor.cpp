#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "General.hpp"
#include "Server.hpp"

extern Server server;

vector<string> split(const string &str, const char &delim) {
	vector<string> tokens;
	string token;
	for (const char &c : str) {
		if (c == delim) {
			tokens.push_back(token);
			token.clear();
		} else {
			token += c;
		}
	}
	tokens.push_back(token);
	return tokens;
}

/*
 * !TODO: instead of getting a client here we should get a user that is associated with the client and then do all the
 * processing from there. Maybe even the ChannelMember class while we are at it.
 */

void CAP(const string &args, const int &client) {
	if (args.empty()) {
		server.sendMessage(client, "ERROR :Closing Link: (localhost) [CAP command is invalid]\r\n");
		cerr << "Error: CAP command is invalid" << endl;
		return;
	}
	server.sendMessage(client, "CAP " + args + "\r\n");
}

void NICK(const string &args, const int &client) {
	if (args.empty()) {
		cerr << "Error: NICK packet has no argument" << endl;
		return;
	} else if (args.find(' ') != string::npos) {
		cerr << "Error: NICK packet has a space in the argument" << endl;
		return;
	}
	server.sendMessage(client, "NICK " + args + "\r\n");
}

void USER(const string &args, const int &client) {
	// split LithiumOx 0 * LithiumOx into {username, 0, *, realname}
	vector<string> tokens = split(args, ' ');
	if (tokens.size() < 4) {
		server.sendMessage(client, "ERROR :Closing Link: (localhost) [USER command is invalid]\r\n");
		cerr << "Error: USER packet has less than 4 arguments" << endl;
		return;
	}

	for (auto *u : server.getUsers()) u->printUser();

	cout << "User " << tokens[0] << " has connected" << endl;
}

void PASS(const string &args, const int &client) {
	if (args.empty()) {
		cerr << "Error: PASS packet has less than 1 argument" << endl;
		return;
	}
	if (args == server.getPassword()) {
		server.sendMessage(client, "AUTHENTICATE\r\n");
		cout << "User has the correct password" << endl;
	} else {
		server.sendMessage(client, "ERROR :Closing Link: (localhost) [Authentication failed]\r\n");
		cerr << "Error: User has failed to authenticate expecte: \"" << server.getPassword() << "\" got: \"" << args
			 << "\"" << endl;
	}
}

void INFO(const string &args, const int &client) {
	if (args.empty()) {
		server.sendMessage(client, "ERROR :Closing Link: (localhost) [INFO command is invalid]\r\n");
		cerr << "Error: INFO packet has less than 1 argument" << endl;
		return;
	}
	server.sendMessage(client, "INFO " + args + "\r\n");
}

void JOIN(const string &args, const int &client) {
	if (args.empty()) {
		server.sendMessage(client, "ERROR :Closing Link: (localhost) [JOIN command is invalid]\r\n");
		cerr << "Error: JOIN packet has less than 1 argument" << endl;
		return;
	}
	server.sendMessage(client, "JOIN " + args + "\r\n");
}

void PacketProcessor(const Packet &packet, const int &client) {
	for (const auto &p : packet) {
		for (const auto &s : store) {
			if (p.first == s.type) s.func(p.second, client);
		}
	}
}
