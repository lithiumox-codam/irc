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

#include "Exceptions.hpp"
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
		exit(EXIT_FAILURE);
	}
	if (close(this->socket) == -1) {
		cerr << "Error: close failed" << "\n";
		exit(EXIT_FAILURE);
	}

	this->socket = -1;
}

int User::getSocket() const { return this->socket; }

const string &User::getNickname() const { return this->nickname; }

const string &User::getUsername() const { return this->username; };

const string &User::getRealname() const { return this->realname; }

const string &User::getHostname() const { return this->hostname; }

void User::setNickname(string &nickname) {
	// Name validation
	for (const auto &user : server.getUsers()) {
		if (user.getNickname() == nickname) {
			throw NicknameInUseException(nickname);
		}
	}

	if (nickname.size() < 2) {
		throw ErroneousNicknameException(nickname);
	}
	if (nickname.size() > 12) {
		nickname = nickname.substr(0, 12);
	}
	if (isdigit(nickname[0]) > 0) {
		throw ErroneousNicknameException(nickname);
	}
	for (const auto &c : nickname) {
		if (isprint(c) == 0 || c == ' ') {
			throw ErroneousNicknameException(nickname);
		}
	}

	this->nickname = nickname;
}

void User::setUsername(string &username) {
	if (username.size() < 2) {
		throw ErroneousUsernameException(username);
	}
	if (username.size() > 16) {
		username = username.substr(0, 12);
	}
	for (const auto &c : username) {
		if (isprint(c) == 0 || c == ' ') {
			throw ErroneousUsernameException(username);
		}
	}

	this->username = username;
}

void User::setRealname(string &realname) { this->realname = realname; }

void User::setHostname(string &hostname) { this->hostname = hostname; }

void User::addHandshake(unsigned int handshake) { this->handshake |= handshake; }

unsigned int User::getHandshake() const { return this->handshake; }

bool User::hasHandshake(unsigned int handshake) const { return (this->handshake & handshake) == handshake; }

void User::readFromSocket() {
	char buffer[UserConfig::BUFFER_SIZE];
	int ret = recv(this->socket, buffer, sizeof(buffer) + 1, 0);

	if (ret == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			return ;
		}
		throw (UserQuitException("Unexpected error in recv:" + string(strerror(errno))));
	}

	if (ret == 0) {
		throw (UserQuitException("Connection lost"));
	}

	buffer[ret] = '\0';
	this->in_buffer.append(buffer, ret);
}

void User::sendToSocket() {
	while (!this->out_buffer.empty()) {
		int ret = send(this->socket, this->out_buffer.data(), this->out_buffer.size(), 0);

		if (ret == -1) {
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
				return ;
			}
			throw (UserQuitException("Unexpected error in send:" + string(strerror(errno))));
		}

		if (ret == 0) {
			throw (UserQuitException("Connection lost"));
		}

		this->out_buffer.erase(0, ret);
	}

	server.epollChange(this->socket, EPOLLIN);
}

void User::addToBuffer(const string &data) {
	this->out_buffer.append(data);

	if (!this->out_buffer.empty()) {
		server.epollChange(this->socket, EPOLLIN | EPOLLOUT);
	}
};

ostream &operator<<(std::ostream &stream, const User &user) {
	const int WIDTH = 52;
	const std::map<unsigned int, char> handshakeMap = {
		{H_INFO, 'I'}, {H_USER, 'U'}, {H_NICK, 'N'}, {H_PASS, 'P'}, {H_WELCOME, 'W'}};

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
