#pragma once

#define RPL_WELCOME "001"	// Welcome message
#define RPL_YOURHOST "002"	// Your host is...(Probably local lol)
#define RPL_CREATED "003"	// Server created at...
#define RPL_MYINFO "004"	// Server info
#define RPL_ISUPPORT "005"	// Server supports this and that (CAP)

#define RPL_TRACELINK "200"		   // Trace link
#define RPL_TRACECONNECTING "201"  // Trace connecting
#define RPL_TRACEHANDSHAKE "202"   // Trace handshake
#define RPL_TRACEUNKNOWN "203"	   // Trace unknown
#define RPL_TRACEOPERATOR "204"	   // Trace operator
#define RPL_TRACEUSER "205"		   // Trace user
#define RPL_TRACESERVER "206"	   // Trace server
#define RPL_STATSLINKINFO "211"	   // Stats link info
#define RPL_STATSCOMMANDS "212"	   // Stats commands
#define RPL_STATSCLINE "213"	   // Stats C line
#define RPL_STATSNLINE "214"	   // Stats N line
#define RPL_STATSILINE "215"	   // Stats I line
#define RPL_STATSKLINE "216"	   // Stats K line
#define RPL_ENDOFSTATS "219"	   // End of stats report
#define RPL_UMODEIS "221"		   // User mode is
#define RPL_STATSLLINE "241"	   // Stats L line
#define RPL_STATSUPTIME "242"	   // Stats uptime
#define RPL_STATSOLINE "243"	   // Stats O line
#define RPL_LUSERCLIENT "251"	   // Luser client
#define RPL_LUSEROP "252"		   // Luser operator count
#define RPL_LUSERUNKNOWN "253"	   // Luser unknown connections
#define RPL_LUSERCHANNELS "254"	   // Luser channels count
#define RPL_LUSERME "255"		   // Luser me
#define RPL_ADMINME "256"		   // Admin me
#define RPL_ADMINLOC1 "257"		   // Admin location 1
#define RPL_ADMINLOC2 "258"		   // Admin location 2
#define RPL_ADMINEMAIL "259"	   // Admin email
#define RPL_LOCALUSERS "265"	   // Local users
#define RPL_GLOBALUSERS "266"	   // Global users
#define RPL_AWAY "301"			   // User away message
#define RPL_USERHOST "302"		   // User hostnames
#define RPL_ISON "303"			   // Users currently online
#define RPL_UNAWAY "305"		   // You are no longer marked as away
#define RPL_NOWAWAY "306"		   // You have been marked as away
#define RPL_WHOISUSER "311"		   // Whois user information
#define RPL_WHOISSERVER "312"	   // Whois server information
#define RPL_WHOISOPERATOR "313"	   // Whois operator information
#define RPL_WHOISIDLE "317"		   // Whois idle time
#define RPL_ENDOFWHOIS "318"	   // End of WHOIS list
#define RPL_WHOISCHANNELS "319"	   // Whois channels list
#define RPL_LISTSTART "321"		   // Channel list start
#define RPL_LIST "322"			   // Channel list
#define RPL_LISTEND "323"		   // End of channel list
#define RPL_CHANNELMODEIS "324"	   // Channel mode is
#define RPL_CREATIONTIME "329"	   // Channel creation time
#define RPL_NOTOPIC "331"		   // No topic set
#define RPL_TOPIC "332"			   // Topic for the channel
#define RPL_INVITING "341"		   // Invitation sent
#define RPL_SUMMONING "342"		   // Summoning user
#define RPL_INVITELIST "346"	   // Invite list
#define RPL_ENDOFINVITELIST "347"  // End of invite list
#define RPL_EXCEPTLIST "348"	   // Exception list
#define RPL_ENDOFEXCEPTLIST "349"  // End of exception list
#define RPL_VERSION "351"		   // Version reply
#define RPL_WHOREPLY "352"		   // WHO reply
#define RPL_NAMREPLY "353"		   // Names list reply
#define RPL_LINKS "364"			   // Links list
#define RPL_ENDOFLINKS "365"	   // End of links list
#define RPL_ENDOFNAMES "366"	   // End of names list
#define RPL_BANLIST "367"		   // Ban list
#define RPL_ENDOFBANLIST "368"	   // End of ban list
#define RPL_ENDOFWHOWAS "369"	   // End of WHOWAS
#define RPL_INFO "371"			   // Information
#define RPL_MOTD "372"			   // Message of the day (MOTD)
#define RPL_ENDOFINFO "374"		   // End of INFO list
#define RPL_MOTDSTART "375"		   // MOTD start
#define RPL_ENDOFMOTD "376"		   // End of MOTD
#define RPL_YOUREOPER "381"		   // You are now an IRC operator
#define RPL_REHASHING "382"		   // Rehashing the server
#define RPL_TIME "391"			   // Server time

#define ERR_UNKNOWNERROR "400"		// Unknown error
#define ERR_NOSUCHNICK "401"		// Nickname not found
#define ERR_NOSUCHSERVER "402"		// No such server
#define ERR_NOSUCHCHANNEL "403"		// No such channel
#define ERR_CANNOTSENDTOCHAN "404"	// Cannot send to channel
#define ERR_TOOMANYCHANNELS "405"	// Too many channels
#define ERR_WASNOSUCHNICK "406"		// Nickname not found
#define ERR_TOOMANYTARGETS "407"	// Too many targets
#define ERR_NOORIGIN "409"			// No origin specified
#define ERR_NORECIPIENT "411"		// No recipient specified
#define ERR_NOTEXTTOSEND "412"		// No text to send
#define ERR_NOTOPLEVEL "413"		// No top level domain
#define ERR_WILDTOPLEVEL "414"		// Wildcard in top level domain
#define ERR_UNKNOWNCOMMAND "421"	// Unknown command
#define ERR_NOMOTD "422"			// No MOTD file
#define ERR_NOADMININFO "423"		// No admin info
#define ERR_FILEERROR "424"			// File error
#define ERR_NONICKNAMEGIVEN "431"	// No nickname given
#define ERR_ERRONEUSNICKNAME "432"	// Erroneous nickname
#define ERR_NICKNAMEINUSE "433"		// Nickname in use
#define ERR_NICKCOLLISION "436"		// Nickname collision
#define ERR_USERNOTINCHANNEL "441"	// User not in channel
#define ERR_NOTONCHANNEL "442"		// Not on channel
#define ERR_USERONCHANNEL "443"		// User is already on channel
#define ERR_NOLOGIN "444"			// No login
#define ERR_SUMMONDISABLED "445"	// Summon disabled
#define ERR_USERSDISABLED "446"		// Users disabled
#define ERR_NOTREGISTERED "451"		// Not registered
#define ERR_NEEDMOREPARAMS "461"	// Need more parameters
#define ERR_ALREADYREGISTRED "462"	// Already registered
#define ERR_NOPERMFORHOST "463"		// No permission for host
#define ERR_PASSWDMISMATCH "464"	// Password mismatch
#define ERR_YOUREBANNEDCREEP "465"	// You're banned
#define ERR_KEYSET "467"			// Key set
#define ERR_CHANNELISFULL "471"		// Channel is full
#define ERR_UNKNOWNMODE "472"		// Unknown mode
#define ERR_INVITEONLYCHAN "473"	// Invite only channel
#define ERR_BANNEDFROMCHAN "474"	// Banned from channel
#define ERR_BADCHANNELKEY "475"		// Bad channel key
#define ERR_NOPRIVILEGES "481"		// No privileges
#define ERR_CHANOPRIVSNEEDED "482"	// Channel operator privileges needed
#define ERR_CANTKILLSERVER "483"	// Can't kill server
#define ERR_RESTRICTED "484"		// Restricted
#define ERR_UNIQOPRIVSNEEDED "485"	// Unique operator privileges needed
#define ERR_UMODEUNKNOWNFLAG "501"	// Unknown user mode flag
#define ERR_USERSDONTMATCH "502"	// Users don't match
