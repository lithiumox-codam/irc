#pragma once

#include <exception>
#include <string>
#include "Channel.hpp"
#include "IRStream.hpp"
#include "User.hpp"
#include "Codes.hpp"

using namespace std;

class IrcException : public exception {
	protected:
		const string message;
		const string code;
		IrcException(const string &msg, const string &I_code) : message(msg), code(I_code) {}
		~IrcException() {}
   public:
		virtual void e_stream(IRStream &stream, User *user) const = 0;
		const char *what() const noexcept override { return message.c_str(); }
};

class NoSuchUserException : public IrcException {
	public:
		NoSuchUserException() : IrcException("No such nickname", ERR_NOSUCHNICK) {}
		void e_stream(IRStream &stream, User *user) const override{
			stream.prefix().code(code).param(user->getNickname()).trail(message).end();
		}
};

class NoSuchChannelException : public IrcException {
	public:
		NoSuchChannelException() : IrcException("No such channel", ERR_NOSUCHCHANNEL) {}
		void e_stream(IRStream &stream, User *user) const override {
			stream.prefix().code(code).param(user->getNickname()).trail(message).end();
		}
};

class NotOnChannelException : public IrcException {
	public:
		NotOnChannelException() : IrcException("You're not in that channel", ERR_NOTONCHANNEL) {}
		void e_stream(IRStream &stream, User *user) const override {
			stream.prefix().code(code).param(user->getNickname()).trail(message).end();
		}
};

class UserNotOperatorException : public IrcException {
	public:
		UserNotOperatorException() : IrcException("You're not a channel operator", ERR_CHANOPRIVSNEEDED) {}
		void e_stream(IRStream &stream, User *user) const override {
			stream.prefix().code(code).param(user->getNickname()).trail(message).end();
		}
};
