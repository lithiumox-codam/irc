#pragma once

#include "General.hpp"
#include "Server.hpp"

class IRStream : public stringstream {
   private:
	vector<string> buff;

   public:
	IRStream() {}
	IRStream &command(const string &cmd);
	IRStream &param(const string &param);
	IRStream &trail(const string &trail);
	void end();

	string getPacket();
	void clear();
};
