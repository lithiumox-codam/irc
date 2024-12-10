#include "../include/Bot.hpp"
#include "../include/Epoll.hpp"

#include <sys/socket.h>
#include <sys/epoll.h>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>

#include <unistd.h>

using namespace std;

extern EpollClass	myEpoll;

// Constructors

Bot::Bot() : socketfd(-1) {}

Bot &Bot::operator=(const Bot &other) {
	if (this != &other) {
		this->socketfd = other.socketfd;
		this->in_buffer = other.in_buffer;
		this->out_buffer = other.out_buffer;
	}
	return *this;
}

Bot::Bot(const Bot &other) {
	*this = other;
}

Bot::~Bot() {
	if (this->socketfd == -1) {
		return;
	}

	if (close(socketfd) == -1) {
		cerr << "Shutting down the bot failed: " << strerror(errno) << '\n';
		exit(EXIT_FAILURE);
	}
	
	this->socketfd = -1;
}

// Server Functions
void Bot::readFromServer(void) {
	char	buffer[4096];
	int		ret;

	ret = recv(this->socketfd, buffer, sizeof(buffer), 0);

	if (ret == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return;
		}
		throw ExecutionException("Unexpected error in recv:" + string(strerror(errno)));
	}

	if (ret == 0) {
		throw ExecutionException("Connection to server lost...");
	}

	buffer[ret] = '\0';
	this->in_buffer.append(buffer, ret);
	cout << "DEBUG: Received: " << this->in_buffer << '\n';

	this->parse();
}

void Bot::sendToServer(void) {
	cout << "DEBUG: Sending: " << this->out_buffer << '\n';

	// Send the buffer
	int ret = send(this->socketfd, this->out_buffer.data(), this->out_buffer.size(), 0);

	cout << "DEBUG: Sent " << ret << " bytes" << '\n';

	if (ret == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return;
		}
		throw ExecutionException("Unexpected error in send:" + string(strerror(errno)));
	}

	if (ret == 0) {
		throw ExecutionException("Connection to server lost...");
	}

	this->out_buffer.erase(0, ret);

	// Change the epoll to remove the EPOLLOUT flag
	if (this->out_buffer.empty()) {
		myEpoll.change(this->socketfd, EPOLLIN);
	}
}

void	Bot::addToBuffer(const string &data) {
	this->out_buffer.append(data);
}

void Bot::join(const string &password) {
	cout << "Joining the server..." << '\n';

	// Join the server
	this->addToBuffer("NICK ircbot\r\n");
	this->addToBuffer("USER ircbot 0 * :ircbot\r\n");
	this->addToBuffer("PASS " + password + "\r\n");
	this->addToBuffer("CAP LS\r\n");
	this->addToBuffer("CAP END\r\n");

	// Set the epoll to be ready to write
	myEpoll.change(this->socketfd, EPOLLIN | EPOLLOUT);
}

// Bot Functions

/**
 * @brief Splits a command into parts like the sender, command type, arguments and the message.
 * 
 * @param command The command to split
 * 
 * @return vector<string> A vector containing the parts of the command:
	 * 	[0] 	- sender
	 * 	[1] 	- command type
	 * 	[2-...] - arguments
	 * 	[end] 	- message
 */
static vector<string> getCommandParts(string &command) {
	vector<string>	parts;

	// Remove the : from the sender
	command.erase(0, 1);

	// Extract the message from the command
	string message;
	size_t messagePos = command.find(':');

	if (messagePos != string::npos) {
		message = command.substr(messagePos + 1);
		command.erase(messagePos);
	}

	// Split the command into parts
	while (!command.empty()) {
		size_t pos = command.find(' ');
		if (pos == string::npos) {
			parts.push_back(command);
			break;
		}
		parts.push_back(command.substr(0, pos));
		command.erase(0, pos + 1);
	}

	// Add the message to the end of the parts
	if (!message.empty()) {
		parts.push_back(message);
	}

	return parts;
}

static vector<string> getCommands(string &buffer, const string &delim) {
	vector<string> commands;
	size_t delimPos = 0;

	while (!buffer.empty())
	{
		delimPos = buffer.find(delim);
		if (delimPos == string::npos) {
			break;
		}
		commands.push_back(buffer.substr(0, delimPos));
		buffer.erase(0, delimPos + delim.size());
	}

	return commands;
}

// :<this->socketfd> 433 <username> <nickname> :Nickname is already in use
// :ircbot!ircbot@localhost JOIN #bot
// :opelser!olebol@localhost JOIN :#bot
// :opelser!olebol@localhost PRIVMSG #bot :hello bot!
// :opelser!olebol@localhost PART #bot :Leaving

static string getNick(const string &sender) {
	size_t nickEnd = sender.find('!');

	if (nickEnd == string::npos) {
		return sender;
	}

	return sender.substr(0, nickEnd);
}

static string replyJOIN(const vector<string> &parts) {
	string nick = getNick(parts[0]);

	if (nick == "ircbot") {
		cout << "DEBUG: Joined the channel" << '\n';
	} else {
		cout << "DEBUG: " << nick << " joined the channel" << '\n';
		return "PRIVMSG " + parts[2] + " :Hello " + nick + ", welcome to the channel!\r\n";
	}
	return string();
}

static string replyPRIVMSG(vector<string> &parts) {
	string nick = getNick(parts[0]);
	string message = parts.back();

	cout << "DEBUG: " << nick << " said: " << message << '\n';

	string response = getGPTResponse(nick, message);

	return "PRIVMSG " + parts[2] + " :" + response + "\r\n";
}

static string replyPART(const vector<string> &parts) {
	string nick = getNick(parts[0]);

	cout << "DEBUG: " << nick << " left the channel" << '\n';

	return "PRIVMSG " + parts[2] + " :Goodbye " + nick + "! Have a nice day!\r\n";
}

static string replyINVITE(const vector<string> &parts) {
	cout << parts[0] << " invited me to " << parts[3] << '\n';
	string channel = parts[3].starts_with(':') ? parts[3].substr(1) : parts[3];
	string response;

	response = "JOIN " + channel + "\r\n";
	response += "PRIVMSG " + channel + " :Hello! I'm ircbot. Thanks for inviting me, " + getNick(parts[0]) + "!\n I'm here to help you with your questions. Ask me anything!\r\n";

	return response;
}

void Bot::parse(void) {
	vector<string> commands = getCommands(this->in_buffer, "\r\n");
	for (string &command : commands) {
		vector<string> parts = getCommandParts(command);

		string response;

		if (parts[1] == "433") {
			cerr << "Error: Nickname is already in use" << '\n';
			exit(EXIT_FAILURE);
		}

		else if (parts[1] == "JOIN") {
			response = replyJOIN(parts);
		}

		else if (parts[1] == "PRIVMSG") {
			response = replyPRIVMSG(parts);
		}

		else if (parts[1] == "PART") {
			response = replyPART(parts);
		}

		else if (parts[1] == "INVITE") {
			response = replyINVITE(parts);
		}
		
		if (!response.empty()) {
			this->addToBuffer(response);
			myEpoll.change(this->socketfd, EPOLLIN | EPOLLOUT);
		}
	}
}
