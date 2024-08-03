#include "General.hpp"

typedef struct func_arr {
	const string key;
	string (*func)(const string &);
} func_arr;

string cap(const string &str) { return str; }

string nick(const string &str) { return str; }

string user(const string &str) { return str; }

string pass(const string &str) { return str; }

string info(const string &str) { return str; }

string join(const string &str) { return str; }

static const func_arr funcs[] = {{"CAP", &cap},	  {"NICK", &nick}, {"USER", &user}, {"PASS", &pass},
								 {"INFO", &info}, {"JOIN", &join}, {"", nullptr}};

string parse(const string &str) {
	for (unsigned long i = 0; funcs[i].func != nullptr; i++) {
		if (str.find(funcs[i].key) != string::npos) {
			return funcs[i].func(str);
		}
	}
	return "";
}
