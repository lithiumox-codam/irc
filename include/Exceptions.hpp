#pragma once

#include <exception>
#include <stdexcept>
#include <string>

#include "Channel.hpp"
#include "Codes.hpp"
#include "IRStream.hpp"
#include "User.hpp"

using namespace std;

class IrcException : public exception {
   protected:
	const string message;
	const string code;
	const string param;

   public:
	IrcException(const string &msg, const string &I_code) : message(msg), code(I_code) {}
	IrcException(const string &msg, const string &I_code, const string &I_param) : message(msg), code(I_code), param(I_param) {}
	~IrcException() {}
	const string &GetCode() const { return code; }
	virtual void e_stream(IRStream &stream, User *user) const = 0;
	const char *what() const noexcept override { return message.c_str(); }
};

class NoSuchUserException : public IrcException {
   public:
	NoSuchUserException(const string &param) : IrcException("No such nickname", ERR_NOSUCHNICK, param) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).param(param).trail(message).end();
	}
};

class NoSuchChannelException : public IrcException {
   public:
	NoSuchChannelException(const string &param) : IrcException("No such channel", ERR_NOSUCHCHANNEL, param) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).param(param).trail(message).end();
	}
};

class NotOnChannelException : public IrcException {
   public:
	NotOnChannelException(const string &param) : IrcException("You're not in that channel", ERR_NOTONCHANNEL, param) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).param(param).trail(message).end();
	}
};

class UserNotOperatorException : public IrcException {
   public:
	UserNotOperatorException() : IrcException("You're not a channel operator", ERR_CHANOPRIVSNEEDED) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).trail(message).end();
	}
};

class UserAlreadyOnChannelException : public IrcException {
   public:
	UserAlreadyOnChannelException() : IrcException("User is already on channel", ERR_USERONCHANNEL) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).trail(message).end();
	}
};

class ChannelFullException : public IrcException {
   public:
	ChannelFullException(const string &param) : IrcException("Channel is full", ERR_CHANNELISFULL, param) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).param(param).trail(message).end();
	}
};

class InviteOnlyChannelException : public IrcException {
   public:
	InviteOnlyChannelException() : IrcException("Cannot join channel (Invite only)", ERR_INVITEONLYCHAN) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).trail(message).end();
	}
};

class BadChannelKeyException : public IrcException {
   public:
	BadChannelKeyException(const string &param) : IrcException("Cannot join channel (+k) - bad key", ERR_BADCHANNELKEY, param) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).param(param).trail(message).end();
	}
};

class NotEnoughParametersException : public IrcException {
   public:
	NotEnoughParametersException() : IrcException("Not enough parameters", ERR_NEEDMOREPARAMS) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).trail(message).end();
	}
};

class UserNotOnChannelException : public IrcException {
   public:
	UserNotOnChannelException(const string &param) : IrcException("User not in channel", ERR_USERNOTINCHANNEL, param) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).param(param).trail(message).end();
	}
};

class CannotSendToChannelException : public IrcException {
   public:
	CannotSendToChannelException() : IrcException("Cannot send to channel missing voice! (+m)", ERR_CANNOTSENDTOCHAN) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).trail(message).end();
	}
};

class NoRecipientException : public IrcException {
   public:
	NoRecipientException() : IrcException("No recipient", ERR_NORECIPIENT) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).trail(message).end();
	}
};

class NoOtherUserModeException : public IrcException {
   public:
	NoOtherUserModeException() : IrcException("Cannot see modes for other users", ERR_USERSDONTMATCH) {}
	void e_stream(IRStream &stream, User *user) const override {
		stream.prefix().code(code).param(user->getNickname()).trail(message).end();
	}
};
