#include "Channel.hpp"

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
	  modes(channel.modes) {}

Channel &Channel::operator=(const Channel &channel) noexcept {
	this->members = channel.members;
	this->name = channel.name;
	this->password = channel.password;
	this->topic = channel.topic;
	this->modes = channel.modes;
	return *this;
}

const string &Channel::getName() const { return this->name; }

const string &Channel::getPassword() const { return this->password; }

void Channel::setPassword(const string &password) {
	this->password = password;
	this->modes.addModes(M_PASSWORD);
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
	this->members.emplace_back(user, Modes(Type::USER));
	if (this->hasOperator(user)) {
		this->getMembers()->back().second.addModes(M_OPERATOR);
	}
}

void Channel::removeUser(User *user) {
	IRStream stream;

	stream.prefix(user, this).param("PART").param(this->getName()).end();
	// NOLINTNEXTLINE
	for (auto it = this->members.begin(); it != this->members.end(); ++it) {
		if (it->first->getSocket() == user->getSocket()) {
			broadcast2(stream, user);

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

std::deque<pair<User *, Modes>> *Channel::getMembers() { return &this->members; }

std::pair<User *, Modes> *Channel::getMember(User *user) {
	for (auto &member : this->members) {
		if (member.first->getSocket() == user->getSocket()) {
			return &member;
		}
	}
	return nullptr;
}

pair<User *, Modes> *Channel::getMember(const string &nickname) {
	// NOLINTNEXTLINE
	for (auto &member : this->members) {
		if (member.first->getNickname() == nickname) {
			return &member;
		}
	}
	throw UserNotOnChannelException();
}

void Channel::broadcast(User *user, const string &message) {
	IRStream stream;

	stream.prefix(user, this).param("PRIVMSG").param(this->getName()).trail(message).end();

	for (auto &member : *this->getMembers()) {
		if (member.first->getSocket() != user->getSocket()) {
			stream.sendPacket(member.first);
		}
	}
}

void Channel::broadcast2(IRStream &stream, User *user) {
	for (auto &member : *this->getMembers()) {
		if (member.first->getSocket() != user->getSocket()) {
			stream.sendPacket(member.first);
		}
	}
}

string Channel::getUserModes(User *user) {
	for (const auto &member : this->members) {
		if (member.first->getSocket() == user->getSocket()) {
			return member.second.getModesString();
		}
	}
	return "";
}

const string &Channel::getTopic() const { return this->topic; }

void Channel::setTopic(const string &topic) { this->topic = topic; }

time_t Channel::getCreated() const { return this->created; }
