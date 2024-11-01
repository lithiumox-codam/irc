#pragma once

#include <iostream>
#include <string>

using namespace std;

class Bot { 
	private:
		string	in_buffer;
		string	out_buffer;

	public:
		Bot();
		Bot &operator=(const Bot &other);
		Bot(const Bot &other);
		~Bot();

		// Server functions
		void			readFromServer(void);
		void			sendToServer(void);
		void			addToBuffer(const string &data);
		void 			join(void);

		// Bot functions
		static string	getResponse(const string &query);
} ;
