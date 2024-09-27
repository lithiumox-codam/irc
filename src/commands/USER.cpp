#include "User.hpp"

#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>

#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

/**
 * @brief Get the Hostname From Socket object
 * @remark This function gets the hostname from a socket file descriptor. So it can be used to get the hostname of a
 * connected client. We do this so the user cannot fake their hostname.
 *
 * @param socket_fd The socket file descriptor.
 * @return std::string The hostname of the socket.
 */
std::string getHostnameFromSocket(int socket_fd) {
	struct sockaddr_storage addr;
	socklen_t len = sizeof(addr);
	char host[NI_MAXHOST];

	if (getpeername(socket_fd, (struct sockaddr *)&addr, &len) == -1) {
		perror("getpeername");
		return "";
	}

	if (getnameinfo((struct sockaddr *)&addr, len, host, sizeof(host), NULL, 0, NI_NAMEREQD) != 0) {
		perror("getnameinfo");
		return "";
	}

	return std::string(host);
}

/**
 * @brief Makes it so that the realname is not split by spaces.
 *
 * @param args The split arguments of the USER command.
 * @return The arguments of the USER command with the realname as one string.
 */
static void correct_realname(vector<string> &args) {
	string realname;
	for (size_t i = 3; i < args.size(); i++) {
		realname += args[i];
		if (i + 1 < args.size()) {
			realname += " ";
		}
	}
	realname.erase(0, realname.find_first_not_of(':'));
	args.erase(args.begin() + 3, args.end());
	args.push_back(realname);
}

/**
 * @brief The USER command is used at the beginning of connection to specify the username, hostname, servername and
 * realname of a new user.
 */
bool USER(IRStream &stream, string &args, User &user) {
	if (user.hasHandshake(U_USER)) {
		stream.code(ERR_ALREADYREGISTRED).param(user.getNickname()).trail("You may not reregister").end();
		return false;
	}
	vector<string> tokens = split(args, ' ');
	correct_realname(tokens);
	if (tokens.size() < 4) {
		cerr << "Error: USER packet has less than 4 arguments" << "\n";
		return false;
	}
	string hostname = getHostnameFromSocket(user.getSocket());

	user.setHostname(hostname);
	user.setUsername(tokens[0]);
	user.setRealname(tokens[3]);
	user.addHandshake(U_USER);
	return true;
}
