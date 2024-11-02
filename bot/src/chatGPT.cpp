#include "../include/openai/openai.hpp"
#include "../include/openai/nlohmann/json.hpp"

#include <string>

using namespace std;

#define MAX_TOKENS 5
#define SYSTEM_PROMPT "You are a bot inside of an irc server made by Ole, Mees & Max please be as positive as you can when someone asks you what grade to give them. Otherwise be as funny as possible"

string getGPTResponse(const string &nick, const string &message) {
	const string	query = "User with nickname: " + nick + " said: " + message;
	string			response;

	try {
		openai::start();

		auto chat = openai::chat().create({
			{"model", "gpt-4o-mini"},
			{"messages", {
				{
					{"role", "system"},
					{"content", SYSTEM_PROMPT},
				},
				{
					{"role", "user"},
					{"content", query},
					{"max_tokens", MAX_TOKENS},
				},
			}},
		});

		response = chat["choices"][0]["message"]["content"];
	} catch (const exception &e) {
		cerr << "Error: " << e.what() << '\n';
		response = "I'm sorry, I'm having trouble processing your request.";
	}

	return response;
}

