#pragma once

#include <string>

using namespace std;

class User {
   private:
	string username;
	string nickname;

   public:
	User(const string &username, const string &nickname);
	~User();
	const string &getUsername();
	const string &getNickname();
	void setNickname(const string &nickname);
};
