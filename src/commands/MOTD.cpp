#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

/**
 * @brief The MOTD command sends the Message of the Day to the user.
 */
void MOTD(IRStream &stream, string &args, User *user) {
	(void)args;

	if (!user->hasHandshake(USER_AUTHENTICATED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return;
	}

	// Start of MOTD
	stream.prefix()
		.code(RPL_MOTDSTART)
		.param(user->getNickname())
		.trail(":- " + server.getHostname() + " Message of the day -")
		.end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail(":-   _____  _____   _____ ").end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail(":-  |_   _||  __ \\ / ____|").end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail(":-    | |  | |__) | |     ").end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail(":-    | |  |  _  /| |     ").end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail(":-   _| |_ | | \\ \\| |____ ").end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail(":-  |_____||_|  \\_\\\\_____|").end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail(":-                        ").end();

	// Cool welcome message
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(":- Welcome to the Interstellar Relay Chat Network!")
		.end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(":- Your gateway to the digital cosmos: " + server.getHostname())
		.end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail(":- ").end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(":- 🌟 Connect with beings across the universe")
		.end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail(":- 🚀 Explore channels at lightspeed").end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(":- 🌌 Share ideas that transcend space and time")
		.end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail(":- ").end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(":- Remember: In space, everyone can hear you type!")
		.end();

	// End of MOTD
	stream.prefix()
		.code(RPL_ENDOFMOTD)
		.param(user->getNickname())
		.trail(":- End of /MOTD command. Enjoy your cosmic journey!")
		.end();
}
