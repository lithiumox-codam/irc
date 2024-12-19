#include "../include/Bot.hpp"
#include "../include/Epoll.hpp"

#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <cstring>
#include <string>
#include <iostream>
#include <iomanip>

#include <functional>
#include <vector>
#include <map>

#define ERR_NICKNAMEINUSE "433"		 // Nickname in use
#define ERR_PASSWDMISMATCH "464"	 // Password mismatch
#define RPL_ENDOFMOTD "376"			 // End of MOTD

using CommandHandler = std::function<string(const vector<string> &)>;

using namespace std;

#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"

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

	this->parse();
}

void Bot::sendToServer(void) {
	// Send the buffer
	int ret = send(this->socketfd, this->out_buffer.data(), this->out_buffer.size(), 0);

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
	cout << "Joining the server as [ircbot] with password [" << password << "]..." << "\n\n\n";

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

#define WIDTH 12
// :<this->socketfd> 433 <username> <nickname> :Nickname is already in use
// :ircbot!ircbot@localhost JOIN #bot
// :opelser!olebol@localhost JOIN :#bot
// :opelser!olebol@localhost PRIVMSG #bot :hello bot!
// :opelser!olebol@localhost PART #bot :Leaving
static void logMessage(const string &nick, const string &channel, const string &message) {
	// Set width for nickname
	int width;

	width = nick.size() > WIDTH ? nick.size() : WIDTH;
	cout << YELLOW << setw(width) << left << "[" + nick + "] " << RESET;

	// Set width for channel
	width = channel.size() > WIDTH ? channel.size() : WIDTH;
	cout << RED << setw(width) << left << "[" + channel + "] " << RESET;

	// Print the message
	cout << message << '\n';
}

static string getNick(const string &sender) {
	size_t nickEnd = sender.find('!');

	if (nickEnd == string::npos) {
		return sender;
	}

	return sender.substr(0, nickEnd);
}

static string JOIN(const vector<string> &parts) {
	string nick = getNick(parts[0]);
	string response;

	if (nick == "ircbot") {
		response = "Hello! I'm ircbot. Thanks for inviting me! Ask me anything!";
		logMessage(nick, "server", "Joined channel " + parts[2]);
	}
	else {
		response = "Hello " + nick + ", welcome to the channel!";
		logMessage("server", parts[2], nick + " joined the channel");
	}

	logMessage("ircbot", parts[2], response);

	return "PRIVMSG " + parts[2] + " :" + response + "\r\n";
}

static string PRIVMSG(const vector<string> &parts) {
	string nick = getNick(parts[0]);
	const string &message = parts.back();

	string response = getGPTResponse(nick, message);

	logMessage(nick, parts[2], message);
	logMessage("ircbot", parts[2], response);

	return "PRIVMSG " + parts[2] + " :" + response + "\r\n";
}

static string PART(const vector<string> &parts) {
	string nick = getNick(parts[0]);
	string response = "Goodbye " + nick + "! Have a nice day!";

	logMessage(nick, parts[2], "left the channel");
	logMessage("ircbot", parts[2], response);

	return "PRIVMSG " + parts[2] + response + "\r\n";
}

static string INVITE(const vector<string> &parts) {
	string nick = getNick(parts[0]);
	string channel = parts[3].starts_with(':') ? parts[3].substr(1) : parts[3];

	logMessage(nick, "server", "Invited me to channel " + channel);

	return "JOIN " + channel + "\r\n";
}

static string NICKNAMEINUSE(const vector<string> &parts) {
	string nick = getNick(parts[0]);

	throw ExecutionException("Nickname << " + nick + " >> is already in use");
}

static string PASSWDMISMATCH(const vector<string> &parts) {
	(void)parts;
	throw ExecutionException("Password mismatch");
}

static string ENDOFMOTD(const vector<string> &parts) {
	(void)parts;
	logMessage("ircbot", "server", "Connected to the server");
	return "";
}

static string getResponse(vector<string> &parts) {
	const string &command = parts[1];

	static const map<string, CommandHandler> handlers = {
		{"JOIN", JOIN},
		{"PRIVMSG", PRIVMSG},
		{"PART", PART},
		{"INVITE", INVITE},
		{ERR_NICKNAMEINUSE, NICKNAMEINUSE},
		{ERR_PASSWDMISMATCH, PASSWDMISMATCH},
		{RPL_ENDOFMOTD, ENDOFMOTD}
	};

	auto handler = handlers.find(command);

	if (handler != handlers.end()) {
		return handler->second(parts);
	}

	return "";
}

void Bot::parse(void) {
	vector<string> commands = getCommands(this->in_buffer, "\r\n");
	for (string &command : commands) {
		vector<string> parts = getCommandParts(command);

		if (parts.size() < 2) {
			cerr << "Warning: Redeived an invalid command: [" << command << "]" << '\n';
			continue ;
		}

		string response = getResponse(parts);

		if (!response.empty()) {
			this->addToBuffer(response);
			myEpoll.change(this->socketfd, EPOLLIN | EPOLLOUT);
		}
	}
}
