#include "General.hpp"
#include "Server.hpp"

extern Server server;

bool MOTD(stringstream &stream, string &args, User &user) {
	if (args.empty()) {
		stream << startRes(RPL_MOTDSTART) << user.getNickname() << " :- " << server.getHostname()
			   << " Message of the day - " << END;

		// ASCII art banner
		stream << startRes(RPL_MOTD) << user.getNickname() << " :-   _____  _____   _____ " << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :-  |_   _||  __ \\ / ____|" << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :-    | |  | |__) | |     " << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :-    | |  |  _  /| |     " << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :-   _| |_ | | \\ \\| |____ " << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :-  |_____||_|  \\_\\\\_____|" << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :-                        " << END;

		// Cool welcome message
		stream << startRes(RPL_MOTD) << user.getNickname() << " :- Welcome to the Interstellar Relay Chat Network!"
			   << END;
		stream << startRes(RPL_MOTD) << user.getNickname()
			   << " :- Your gateway to the digital cosmos: " << server.getHostname() << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :- " << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :- 🌟 Connect with beings across the universe" << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :- 🚀 Explore channels at lightspeed" << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :- 🌌 Share ideas that transcend space and time" << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :- " << END;
		stream << startRes(RPL_MOTD) << user.getNickname() << " :- Remember: In space, everyone can hear you type!"
			   << END;

		stream << startRes(RPL_ENDOFMOTD) << user.getNickname() << " :End of /MOTD command. Enjoy your cosmic journey!"
			   << END;
		return true;
	}
	stream.str("");
	stream << startRes(ERR_NEEDMOREPARAMS) << user.getNickname() << " :Outside of project scope!" << END;
	return false;
}
