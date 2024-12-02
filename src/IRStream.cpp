#include "IRStream.hpp"

#include <iostream>

#include "Server.hpp"

extern Server server;

IRStream::IRStream() {}

IRStream::~IRStream() { clear(); }

/**
 * @brief Sets the current command to the given command.
 * @code " <command>" cannot contain spaces.
 * @warning This will clear the current command. Only use this if you want to set the command to something else. Its
 * set by default to whatever the parser is currently parsing.
 *
 * @param cmd The command to set the current command to.
 * @return IRStream& The current IRStream.
 */
IRStream &IRStream::setCommand(const string &cmd) {
	currentCommand = cmd;
	return *this;
}

/**
 * @brief Adds the generic server prefix to the current command.
 * @code ":<hostname>" cannot contain spaces.
 *
 * @return IRStream& The current IRStream.
 */
IRStream &IRStream::prefix() {
	*this << ":" << server.getHostname();
	return *this;
}

/**
 * @brief Add the user prefix to the current command.
 * @code " :<nickname>!<username>@<hostname>" cannot contain spaces.
 *
 * @param user A pointer to the user to prefix the command with.
 * @return IRStream&
 */
IRStream &IRStream::prefix(User *user) {
	*this << ":" << user->getNickname() << "!" << user->getUsername() << "@" << user->getHostname();
	return *this;
}

/**
 * @brief Adds the modes the user possesses in a channel to the current command.
 *
 * @param user A pointer to the user to prefix the command with.
 * @param channel A pointer to the channel the user is in.
 * @return IRStream&
 */
IRStream &IRStream::prefix(User *user, Channel *channel) {
	auto isOperator = channel->getMember(user)->second.hasModes(M_OPERATOR);
	*this << ":" << (isOperator ? "@" : "") << user->getNickname() << "!" << user->getUsername() << "@"
		  << user->getHostname();
	return *this;
}

/**
 * @brief Add a code to the current command.
 * @code " <code>" cannot contain spaces.
 *
 * @param code
 * @return IRStream&
 */
IRStream &IRStream::code(const string &code) {
	*this << " " << code;
	return *this;
}

/**
 * @brief Injects the current command into the buffer. Useful for commands like CAP or NICK that include it in the
 * repsonse.
 *
 * @return IRStream&
 */
IRStream &IRStream::command() {
	*this << " " << currentCommand;
	return *this;
}

/**
 * @brief Add a parameter to the current command.
 * @code " <param>" cannot contain spaces.
 * @param param
 * @return IRStream& The current IRStream.
 */
IRStream &IRStream::param(const string &param) {
	*this << " " << param;
	return *this;
}

/**
 * @brief Add a vector of parameters to the current command.
 * @code " <param>" cannot contain spaces.
 *
 * @param params The vector of parameters to add to the current command.
 * @return IRStream& The current IRStream.
 */
IRStream &IRStream::params(const vector<string> &params) {
	for (const auto &param : params) {
		*this << " " << param;
	}
	return *this;
}

IRStream &IRStream::param(const char &param) {
	*this << " " << param;
	return *this;
}

/**
 * @brief Add a trail to the current command.
 * @code " :<trail>" can contain spaces.
 *
 * @param trail The trail to add to the current command.
 * @return IRStream& The current IRStream.
 */
IRStream &IRStream::trail(const string &trail) {
	*this << " :" << trail;
	return *this;
}

/**
 * @brief Add a vector of trails to the current command.
 * @code " :<trail>" can contain spaces.
 *
 * @param trail The vector of trails to add to the current command.
 * @return IRStream& The current IRStream.
 */
IRStream &IRStream::trail(const vector<string> &trail) {
	*this << " :";
	for (const auto &str : trail) {
		*this << str << " ";
	}
	return *this;
}

/**
 * @brief Capsulate the current command and add it to the buffer.
 */
IRStream &IRStream::end() {
	if (!str().empty()) {
		parts.push_back(str() + "\r\n");
		str("");
	}
	return *this;
}

/**
 * @brief Assemble the packet from the buffer.
 *
 * @param user The user to send the packet to.
 */
void IRStream::sendPacket(User *user) {
	for (const auto &line : parts) {
		user->addToBuffer(line);
	}
	// NOLINTNEXTLINE
	cout << "[\033[1;" << (31 + (user->getSocket() % 6)) << "m" << user->getNickname() << "\033[0m] "
		 << (user->hasHandshake(H_AUTHENTICATED) ? "🔒" : "") << (user->modes.hasModes(M_OPERATOR) ? "🧏" : "")
		 << "\n"
		 << user->getOutBuffer() << "\n";
}

/**
 * @brief Get the current content of the buffer. Useful for debugging and/or assembling the packet without directly
 * sending it.
 *
 * @return string The current content of the buffer.
 */
string IRStream::getString() {
	string packet;
	for (const auto &line : parts) {
		packet += line;
	}
	return packet;
}

/**
 * @brief Clear the buffer and the current command.
 */
void IRStream::clear() {
	parts.clear();
	currentCommand.clear();
	str("");
}
