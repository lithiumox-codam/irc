#include "IRStream.hpp"

IRStream &IRStream::command(const string &cmd) {
	if (!str().empty()) {
		finalizeCommand();
	}
	*this << ":" << server->getHostname() << " " << cmd;
	return *this;
}

IRStream &IRStream::param(const string &param) {
	*this << " " << param;
	return *this;
}

IRStream &IRStream::trail(const string &trail) {
	*this << " :" << trail;
	return *this;
}

void IRStream::end() {
	buff.push_back(str() + "\r\n");
	str("");
}

string IRStream::getPacket() {
	string packet;

	end();

	for (auto &line : buff) {
		packet += line;
	}
	return packet;
}
