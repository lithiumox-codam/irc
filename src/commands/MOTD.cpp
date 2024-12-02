#include "Codes.hpp"
#include "General.hpp"
#include "IRStream.hpp"
#include "Server.hpp"

extern Server server;

/**
 * @brief The MOTD command sends the Message of the Day to the user.
 */
void MOTD(IRStream &stream, std::string &args, User *user) {
	(void)args;

	if (!user->hasHandshake(USER_AUTHENTICATED)) {
		stream.prefix().code(ERR_NOTREGISTERED).param(user->getNickname()).trail("You have not registered").end();
		return;
	}

	// Start of MOTD
	stream.prefix().code(RPL_MOTDSTART).param(user->getNickname()).end();

	// Custom ASCII Art for MOTD using Raw String Literals
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(R"(__/\\\\\\\\\\\____/\\\\\\\\\____________/\\\\\\\\\______________________/\\\_______/\\\\\\\\\_____)")
		.end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(R"(_\/////\\\///___/\\\///////\\\_______/\\\////////_____________________/\\\\\_____/\\\///////\\\___)")
		.end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(R"(_____\/\\\_____\/\\\_____\/\\\_____/\\\/____________________________/\\\/\\\____\///______\//\\\__)")
		.end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(R"(_____\/\\\_____\/\\\\\\\\\\\/_____/\\\____________________________/\\\/\/\\\______________/\\\/___)")
		.end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(R"(_____\/\\\_____\/\\\//////\\\____\/\\\__________________________/\\\/__\/\\\___________/\\\//_____)")
		.end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(R"(_____\/\\\_____\/\\\____\//\\\___\//\\\_______________________/\\\\\\\\\\\\\\\\_____/\\\//________)")
		.end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(R"(_____\/\\\_____\/\\\_____\//\\\___\///\\\____________________\///////////\\\//____/\\\/___________)")
		.end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(R"(__/\\\\\\\\\\\_\/\\\______\//\\\____\////\\\\\\\\\_____________________\/\\\_____/\\\\\\\\\\\\\\\_)")
		.end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail(R"(_\///////////__\///________\///________\/////////______________________\///_____\///////////////__)")
		.end();

	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail("").end();

	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail("").end();
	stream.prefix()
		.code(RPL_MOTD)
		.param(user->getNickname())
		.trail("     Users Online: " + server.getUserCount())
		.end();
	stream.prefix().code(RPL_MOTD).param(user->getNickname()).trail("     Server: " + server.getHostname()).end();

	stream.prefix().code(RPL_ENDOFMOTD).param(user->getNickname()).trail("").end();
}
