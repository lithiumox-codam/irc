#include "Channel.hpp"

#include <cstddef>
#include <utility>

#include "Exceptions.hpp"
#include "IRStream.hpp"
#include "Modes.hpp"
#include "User.hpp"

using namespace std;

Channel::Channel() : modes(Type::CHANNEL) {};

Channel::Channel(const string &name) : name(name), created(time(nullptr)), modes(Type::CHANNEL) {}

Channel::Channel(const Channel &channel) noexcept
	: members(channel.members),
	  name(channel.name),
	  password(channel.password),
	  topic(channel.password),
	  modes(channel.modes), limit(10) {}

Channel &Channel::operator=(const Channel &channel) noexcept {
	this->members = channel.members;
	this->name = channel.name;
	this->password = channel.password;
	this->topic = channel.topic;
	this->modes = channel.modes;
	this->limit = channel.limit;
	return *this;
}

const string &Channel::getName() const { return this->name; }

const string &Channel::getPassword() const { return this->password; }

void Channel::setPassword(const string &password) {
	this->password = password;
	this->modes.add(M_PASSWORD);
}

void Channel::setName(const string &name) { this->name = name; }

/**
 * @brief Adds a user to the channel.
 * @remark When a user is added we check if they are in the operator list in which case their modes are assigned. So if
 * they leave and rejoin they will still have their operator status.
 *
 * @param user The user to add.
 */
void Channel::addUser(User *user) {
	this->members.emplace_back(user, Modes(Type::CHANNELMEMBER));
	if (this->hasOperator(user)) {
		this->getMembers()->back().second.add(M_OPERATOR);
	}
}

void Channel::removeUser(User *user) {
	IRStream stream;

	for (auto it = this->members.begin(); it != this->members.end(); ++it) {
		auto &[memberUser, _] = *it;
		if (memberUser->getSocket() == user->getSocket()) {
			this->members.erase(it);
			return;
		}
	}
}

bool Channel::hasUser(User *user) const {
	// NOLINTNEXTLINE
	for (const auto &member : this->members) {
		if (member.first->getSocket() == user->getSocket()) {
			return true;
		}
	}
	return false;
}

void Channel::addOperator(User *user) { operators.push_back(user); }

void Channel::removeOperator(User *user) {
	// NOLINTNEXTLINE
	for (auto it = this->operators.begin(); it != this->operators.end(); ++it) {
		if ((*it)->getSocket() == user->getSocket()) {
			this->operators.erase(it);
			return;
		}
	}
}

bool Channel::hasOperator(User *user) const {
	// NOLINTNEXTLINE
	for (const auto &op : this->operators) {
		if (op->getSocket() == user->getSocket()) {
			return true;
		}
	}
	return false;
}

void Channel::addInvited(User *user) { this->invited.push_back(user); }

void Channel::removeInvited(User *user) {
	// NOLINTNEXTLINE
	for (auto it = this->invited.begin(); it != this->invited.end(); ++it) {
		if ((*it)->getSocket() == user->getSocket()) {
			this->invited.erase(it);
			return;
		}
	}
}

bool Channel::hasInvited(User *user) const {
	// NOLINTNEXTLINE
	for (const auto &member : this->invited) {
		if (member->getSocket() == user->getSocket()) {
			return true;
		}
	}
	return false;
}

std::deque<Member> *Channel::getMembers() { return &this->members; }

std::Member *Channel::getMember(User *user) {
	for (auto &[member, _] : this->members) {
		if (member->getSocket() == user->getSocket()) {
			return &member;
		}
	}
	return nullptr;
}

Member *Channel::getMember(const string &nickname) {
	// NOLINTNEXTLINE
	for (auto &[member, _] : this->members) {
		if (member->getNickname() == nickname) {
			return &member;
		}
	}
	throw UserNotOnChannelException(nickname);
}

void Channel::broadcast(User *user, const string &message) {
	IRStream stream;

	stream.prefix(user, this).param("PRIVMSG").param(this->getName()).trail(message).end();

	for (auto &[member, _] : *this->getMembers()) {
		if (member->getSocket() != user->getSocket()) {
			stream.sendPacket(member);
		}
	}
}

void Channel::broadcast(IRStream &stream, User *user) {
	for (auto &[member, _] : *this->getMembers()) {
		if (user != NULL && (member->getSocket() != user->getSocket())) {
			stream.sendPacket(member);
		}
	}
}

void Channel::broadcast(IRStream &stream) {
	for (auto &[member, _] : *this->getMembers()) {
		stream.sendPacket(member);
	}
}

string Channel::getUserModes(User *user) {
	for (auto &[member, _] : *this->getMembers()) {
		if (member->getSocket() == user->getSocket()) {
			return member.getString();
		}
	}
	return "";
}

const string &Channel::getTopic() const { return this->topic; }

void Channel::setTopic(const string &topic) { this->topic = topic; }

void Channel::setTopicTime(time_t time) { this->topictime = time; }

time_t Channel::getTopicTime() const { return this->topictime; }

void Channel::setTopicSetter(User *user) { this->topicsetter = user->getNickname(); }

string Channel::getTopicSetter() const { return this->topicsetter; }

time_t Channel::getCreated() const { return this->created; }

void Channel::setLimit(size_t limit) {
	this->limit = limit;
	this->modes.add(M_LIMIT);
}

void Channel::removeLimit() { this->modes.remove(M_LIMIT); }
