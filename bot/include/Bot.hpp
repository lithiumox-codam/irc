#pragma once

#include <string>
#include <stdexcept>

using namespace std;

class Bot { 
	private:
		int		socketfd;
		string	in_buffer;
		string	out_buffer;

	public:
		Bot();
		Bot &operator=(const Bot &other);
		Bot(const Bot &other);
		~Bot();

		// Initialization functions
		void			init(int argc, char **argv);
		void 			createSocket(void);
		void			connectToServer(void);
		void 			join(const string &password);

		// Server functions
		void			readFromServer(void);
		void			sendToServer(void);
		void			addToBuffer(const string &data);

		// Bot functions
		void			parse(void);
} ;

string getGPTResponse(const string &nick, const string &message);

// Exceptions
class SetUpException : public runtime_error {
   public:
	SetUpException(const string &msg) : runtime_error(msg) {}
};

class ExecutionException : public runtime_error {
   public:
	ExecutionException(const string &msg) : runtime_error(msg) {}
};
