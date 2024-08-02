#pragma once

#include <string>

using namespace std;

class User {
   private:
	string username;
	string nickname;

   public:
	void Client(const string &username, const string &nickname);
	~Client();
	void stop();
	void start();
}
