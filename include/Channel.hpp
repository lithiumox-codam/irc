#pragma once

#include <ctime>
#include <deque>
#include <iostream>
#include <string>
#include <utility>

class IRStream;
#include "IRStream.hpp"
#include "Modes.hpp"
#include "User.hpp"

using namespace std;

#define MEMBER_LIMIT 50

typedef pair<User*, Modes> Member;

class Channel {
   private:
	deque<Member> members;
	deque<User *> operators;
	deque<User *> invited;
	string name;
	string password;
	string topic;
	size_t limit;

	time_t created;
	string topicsetter;
	time_t topictime;

   public:
	Modes modes;
	Channel();
	Channel(const string &name);
	Channel(const Channel &channel) noexcept;
	Channel &operator=(const Channel &channel) noexcept;

	[[nodiscard]] const string &getName() const;
	void setName(const std::string &name);

	[[nodiscard]] const string &getPassword() const;
	void setPassword(const std::string &password);

	void setTopic(const string &topic);
	[[nodiscard]] const string &getTopic() const;

	void setTopicTime(time_t time);
	[[nodiscard]] time_t getTopicTime() const;

	void setTopicSetter(User *user);
	[[nodiscard]] string getTopicSetter() const;

	void addUser(User *user);
	void removeUser(User *user);
	bool hasUser(User *user) const;

	void addOperator(User *user);
	void removeOperator(User *user);
	bool hasOperator(User *user) const;

	void addInvited(User *user);
	void removeInvited(User *user);
	bool hasInvited(User *user) const;

	[[nodiscard]] deque<Member> *getMembers();
	[[nodiscard]] Member *getMember(User *user);
	[[nodiscard]] Member *getMember(const string &nickname);

	void broadcast(User *user, const string &message);
	void broadcast(IRStream &stream, User *user);
	void broadcast(IRStream &stream);

	string getUserModes(User *user);

	[[nodiscard]] time_t getCreated() const;

	void setLimit(size_t limit);
	void removeLimit();
};

ostream &operator<<(ostream &stream, Channel &channel);
