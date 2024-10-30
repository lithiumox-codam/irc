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
#include <vector>

#include "General.hpp"
#include "Server.hpp"

extern Server server;

User::User(int socket) : socket(socket), handshake(0) {
	if (server.operatorCheck(this)) {
		modes.addModes(M_OPERATOR);
	}
}

User::User(const User &user) noexcept {
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

void User::setUsername(string &username) { this->username = std::move(username); }

void User::setRealname(string &realname) { this->realname = std::move(realname); }

void User::setHostname(string &hostname) { this->hostname = std::move(hostname); }

void User::addHandshake(unsigned int handshake) { this->handshake |= handshake; }

unsigned int User::getHandshake() const { return this->handshake; }

bool User::hasHandshake(unsigned int handshake) const { return (this->handshake & handshake) == handshake; }

int User::readFromSocket() {
	vector<char> buffer(UserConfig::BUFFER_SIZE);
	int bytesRead = recv(this->socket, buffer.data(), buffer.size(), 0);

	if (bytesRead <= 0) {
		return bytesRead;
	}

	buffer.push_back('\0');

	this->in_buffer.append(buffer.data());

	parse(this); // if this fails, need to remove user
	this->in_buffer.clear();
	return bytesRead;
}

void User::addToBuffer(const string &data) {
	this->out_buffer.append(data);
	server.epollChange(this->socket, EPOLLIN | EPOLLOUT);
};

void User::clearInBuffer() { this->in_buffer.clear(); }

void User::clearOutBuffer() { this->out_buffer.clear(); }

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

int User::sendToSocket() {
	size_t totalSent = 0;
	while (!out_buffer.empty()) {
		ssize_t bytesRead = send(socket, out_buffer.data(), out_buffer.size(), MSG_NOSIGNAL);
		if (bytesRead > 0) {
			out_buffer.erase(0, bytesRead);
			totalSent += bytesRead;
		} else if (bytesRead == 0) {
			// Connection closed by peer
			return -1;
		} else if (bytesRead == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// Socket buffer is full, try again later
				return totalSent > 0 ? totalSent : 0;
			}
			// Other error occurred
			cerr << "Error: send failed: " << strerror(errno) << "\n";
			return -1;
		}
	}
	return totalSent;
}

bool User::checkPacket() { return this->in_buffer.find("\r\n") != string::npos; }

string &User::getInBuffer() { return this->in_buffer; }

string &User::getOutBuffer() { return this->out_buffer; }
