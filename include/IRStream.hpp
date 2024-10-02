#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "Channel.hpp"
#include "User.hpp"

using namespace std;

class IRStream : public stringstream {
   private:
	vector<string> parts;
	string currentCommand;

   public:
	IRStream();
	~IRStream();

	IRStream &setCommand(const string &cmd);
	IRStream &prefix();
	IRStream &prefix(User *user);
	IRStream &prefix(User *user, Channel *channel);
	IRStream &command();
	IRStream &code(const string &code);
	IRStream &param(const string &param);
	IRStream &params(const vector<string> &params);
	IRStream &trail(const string &trail);
	IRStream &end();

	void sendPacket(User *user);
	string getString();
	void clear();
};
