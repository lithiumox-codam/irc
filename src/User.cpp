#include "User.hpp"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <ostream>

#include "General.hpp"
#include "Server.hpp"

extern Server server;

User::User(int socket) : socket(socket), handshake(0), modes(Type::USER) {}

User::User(const User &user) noexcept : modes(user.modes) {
	this->socket = user.socket;
	this->username = user.username;
	this->nickname = user.nickname;
	this->realname = user.realname;
	this->hostname = user.hostname;
	this->handshake = user.handshake;
	this->in_buffer = user.in_buffer;
	this->out_buffer = user.out_buffer;
}

auto User::operator=(const User &user) noexcept -> User & {
	if (this == &user) {
		return *this;
	}

	this->socket = user.socket;
	this->username = user.username;
	this->nickname = user.nickname;
	this->realname = user.realname;
	this->hostname = user.hostname;
	this->handshake = user.handshake;
	this->in_buffer = user.in_buffer;
	this->out_buffer = user.out_buffer;

	return *this;
}

void User::closeSocket() {
	if (this->socket == -1) {
		return;
	}

	if (shutdown(this->socket, SHUT_RDWR) == -1) {
		if (errno == ENOTCONN) {
			cerr << "Error: socket not connected" << "\n";
		} else if (errno == ENOTSOCK) {
			cerr << "Error: socket is not a socket" << "\n";
		} else if (errno == EBADF) {
			cerr << "Error: socket is not a valid file descriptor" << "\n";
		} else {
			cerr << "Error: shutdown failed" << "\n";
		}
		cerr << "Error: shutdown failed" << "\n";
	}
	if (close(this->socket) == -1) {
		cerr << "Error: close failed" << "\n";
	}

	this->socket = -1;
}

int User::getSocket() const { return this->socket; }

const string &User::getNickname() const { return this->nickname; }

const string &User::getUsername() const { return this->username; };

const string &User::getRealname() const { return this->realname; }

const string &User::getHostname() const { return this->hostname; }

void User::setNickname(string &nickname) { this->nickname = std::move(nickname); }

void User::setUsername(string &username) {
	this->username = std::move(username);
	if (server.operatorCheck(this)) {
		this->modes.addModes(M_OPERATOR);
	}
}

void User::setRealname(string &realname) { this->realname = std::move(realname); }

void User::setHostname(string &hostname) { this->hostname = std::move(hostname); }

void User::addHandshake(unsigned int handshake) { this->handshake |= handshake; }

unsigned int User::getHandshake() const { return this->handshake; }

bool User::hasHandshake(unsigned int handshake) const { return (this->handshake & handshake) == handshake; }

bool User::readFromSocket() {
	char buffer[UserConfig::BUFFER_SIZE];
	int ret = recv(this->socket, buffer, sizeof(buffer) + 1, 0);

	if (ret == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return true;
		}
		cerr << "Error: recv(): " << strerror(errno) << '\n';
		return false;
	}

	if (ret == 0) {
		cerr << "Connection to user " << this->getNickname() << " lost..." << '\n';
		return false;
	}

	buffer[ret] = '\0';
	this->in_buffer.append(buffer, ret);
	cout << RED << "DEBUG: Received: " << this->in_buffer << RESET << '\n';

	this->parse();

	return true;
}

bool User::sendToSocket() {
	while (!this->out_buffer.empty()) {
		cout << GREEN << "DEBUG: Sending: " << this->out_buffer << RESET << '\n';

		int ret = send(this->socket, this->out_buffer.data(), this->out_buffer.size(), 0);

		if (ret == -1) {
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
				return true;
			}
			cerr << "Error: send():" << strerror(errno) << '\n';
			return false;
		}

		if (ret == 0) {
			cerr << "DEBUG: User " << this->getNickname() << " gracefully disconnected" << '\n';
			return false;
		}

		this->out_buffer.erase(0, ret);
	}

	server.epollChange(this->socket, EPOLLIN);

	return true;
}

void User::addToBuffer(const string &data) {
	this->out_buffer.append(data);

	if (!this->out_buffer.empty()) server.epollChange(this->socket, EPOLLIN | EPOLLOUT);
};

ostream &operator<<(std::ostream &stream, const User &user) {
	const int WIDTH = 52;
	const std::map<unsigned int, char> handshakeMap = {
		{USER_INFO, 'I'}, {USER_USER, 'U'}, {USER_NICK, 'N'}, {USER_PASS, 'P'}, {USER_WELCOME, 'W'}};

	// NOLINTNEXTLINE
	auto line = [](char l, char m, char r) { return l + std::string(WIDTH - 2, m) + r; };

	auto center = [](const std::string &text, int width) {
		int padding = width - text.length();
		int lpad = padding / 2;
		int rpad = padding - lpad;
		return std::string(lpad, ' ') + text + std::string(rpad, ' ');
	};

	auto formatField = [](const std::string &label, const std::string &value) {
		std::ostringstream oss;
		// NOLINTNEXTLINE
		oss << "| " << std::left << std::setw(10) << label << "| " << std::left << std::setw(37) << value << "|";
		return oss.str();
	};

	std::string handshakeStr;
	for (const auto &pair : handshakeMap) {
		if (user.hasHandshake(pair.first)) {
			handshakeStr += "[" + std::string(1, pair.second) + "] ";
		}
	}
	if (!handshakeStr.empty()) {
		handshakeStr.pop_back();
	}

	stream << line('+', '-', '+') << "\n";
	stream << "|" << center("** User Information **", WIDTH - 2) << "|\n";
	stream << line('+', '-', '+') << "\n";
	stream << formatField("Nickname", user.getNickname()) << "\n";
	stream << formatField("Username", user.getUsername()) << "\n";
	stream << formatField("Realname", user.getRealname()) << "\n";
	stream << formatField("Hostname", user.getHostname()) << "\n";
	stream << formatField("Socket", std::to_string(user.getSocket())) << "\n";
	stream << line('+', '-', '+') << "\n";
	stream << formatField("Handshake", handshakeStr) << "\n";
	stream << line('+', '-', '+') << "\n";

	return stream;
}

string &User::getInBuffer() { return this->in_buffer; }

string &User::getOutBuffer() { return this->out_buffer; }
