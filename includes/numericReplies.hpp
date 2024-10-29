/* Copyright 2024 <yusengok>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:37:02 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/29 08:54:09 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_NUMERICREPLIES_HPP_
#define INCLUDES_NUMERICREPLIES_HPP_

#include <string>

#include "./Channel.hpp"
#include "./Client.hpp"
#include "./serverConfig.hpp"

#define RUNTIME_ERROR "Failed to send numeric reply"

/*--------- Messages definition ----------------------------------------------*/

/* Connection registration */

#define _001_RPL_WELCOME(nick, user, host)                              \
  (std::string(":") + SRV_NAME + " 001 " + nick + " :Welcome to the " + \
  NETWORK_NAME + " Network, " + nick + "!" + user + "@" + host + "\r\n")

#define _002_RPL_YOURHOST(nick)                                       \
  (std::string(":") + SRV_NAME + " 002 " + nick + " :Your host is " + \
  SRV_NAME + ", running version " + SRV_VERSION + "\r\n")

#define _003_RPL_CREATED(nick, starttime)         \
  (std::string(":") + SRV_NAME + " 003 " + nick + \
  " :This server was created on " + starttime + "\r\n")

#define _004_RPL_MYINFO(nick)                                            \
  (std::string(":") + SRV_NAME + " 004 " + nick + " :" + SRV_NAME + " " + \
  SRV_VERSION + " " + USER_MODES + " " + CHANNEL_MODES + "\r\n")
//----- The available user modes -------------------
// +i   Invisible: The user is hidden from /WHO queries unless they are in the same channel.
// +o   Operator: Grants IRC operator privileges to the user.
// +r   Registered: The user has registered with services or identified themselves.
//----- The available channel modes -------------------
// +i   Invite-only: Only invited users can join the channel.
// +t   Topic settable by ops only: Only channel operators can change the topic.
// +k   Key: A password is required to join the channel.
// +o   Operator:
// +l   Limit: Limits the number of users that can join the channel.

#define _005_RPL_ISUPPORT(nick, tokens) \
  (std::string(":") + SRV_NAME + " 005 " + nick + " :" + tokens + \
  " :are supported by this server\r\n")
// Inform clients about the server-supported features and settings

#define _221_RPL_UMODEIS(nick, modes) \
  (std::string(":") + SRV_NAME + " 221 " + nick + " " + modes + "\r\n")
// Informs client's currently-set user modes. (Reply to MODE <nick> command)

/* Channels */

# define _331_RPL_NOTOPIC(nick, chanName) \
  (std::string(":") + SRV_NAME + " 331 " + nick + " " + chanName + \
  " :No topic is set\r\n")
// Indicates that the channel has no topic set.

#define _332_RPL_TOPIC(nick, chanName, topic) \
  (std::string(":") + SRV_NAME + " 332 " + nick + " " + chanName + \
  " :" + topic + "\r\n")
// Current topic of the channel.

#define _333_RPL_TOPICWHOTIME(nick, chanName, author, setTime) \
  (std::string(":") + SRV_NAME + " 333 " + nick + " " + chanName + \
  " " + author + " " + setTime + "\r\n")
// By whom and when(unix timestamp) is the topic set. Sent with 332

#define _336_RPL_INVITELIST(nick, chanName) \
  (std::string(":") + SRV_NAME + " 336 " + nick + " " + chanName + "\r\n")
// Indicate a channel the client was invited to.
// If the client is invited to multiple channels, the server sends multiple replies.
// If there is no channel to which the client was invited, the server sends only 337.
// Sent as a reply to the INVITE command without parameter

#define _337_RPL_ENDOFINVITELIST(nick) \
  (std::string(":") + SRV_NAME + " 337 " + nick + " :End of /INVITE list\r\n")

#define _341_RPL_INVITING(nick, invitedNick, chanName) \
  (std::string(":") + SRV_NAME + " 341 " + nick + + " " + invitedNick + " " + \
  chanName + "\r\n")
// Confirms the user has been invited to the channel.
// Sent as a reply to the INVITE command.

#define _353_RPL_NAMREPLY(nick, chanNameWithSymbol, nicknames) \
  (std::string(":") + SRV_NAME + " 353 " + nick + " " + chanNameWithSymbol + \
  " :" + nicknames + "\r\n")
// : List of users in the channel.

#define _366_RPL_ENDOFNAMES(nick, chanName) \
  (std::string(":") + SRV_NAME + " 366 " + nick + " " + chanName + \
  " :End of /NAMES list\r\n")

//------------------------------------------------------------------------------
// 322 RPL_LIST: Channel list with details.
// 323 RPL_LISTEND: End of channel list.

// 324 RPL_CHANNELMODEIS: Channel mode is displayed. Reply to MODE #channel
// 329_RPL_CREATIONTIME: creation time of a channel. Reply to MODE #channel

// 367_RPL_BANLIST: List of bans set on the channel. (Sent as a reply to the MODE)

// 368_RPL_ENDOFBANLIST: End of the list of bans for the channel.
//------------------------------------------------------------------------------

/* Error messages */

#define _401_ERR_NOSUCHNICK(nick, targetNick) \
  (std::string(":") + SRV_NAME + " 401 " + nick + " " + targetNick + \
  " :No such nick/channel\r\n")
// Indicates that no client can be found for the supplied nickname.

#define _403_ERR_NOSUCHCHANNEL(nick, chanName) \
  (std::string(":") + SRV_NAME + " 403 " + nick + " " + chanName + \
  " :No such channel\r\n")

#define _421_ERR_UNKNOWNCOMMAND(nick, command) \
  (std::string(":") + SRV_NAME + " 421 " + nick + " " + command + \
  " :Unknown command\r\n")

#define _431_ERR_NONICKNAMEGIVEN(nick) \
  (std::string(":") + SRV_NAME + " 431 " + nick + " :No nickname given\r\n")
// Returned when a nickname parameter is expected for a command but isn’t given.

#define _432_ERR_ERRONEUSNICKNAME(nick) \
  (std::string(":") + SRV_NAME + " 432 " + nick + " :Erroneus nickname\r\n")
// The desired nickname contains characters that are disallowed by the server.

#define _433_ERR_NICKNAMEINUSE(nick)              \
  (std::string(":") + SRV_NAME + " 433 " + nick + \
  " :Nickname is already in use\r\n")

#define _442_ERR_NOTONCHANNEL(nick, chanName) \
  (std::string(":") + SRV_NAME + " 442 " + nick + " " + chanName + \
  " :You're not on that channel\r\n")

#define _443_ERR_USERONCHANNEL(nick, invitedNick, chanName) \
  (std::string(":") + SRV_NAME + " 443 " + nick + " " + invitedNick + " " + \
  chanName + " :is already on channel\r\n")

#define _461_ERR_NEEDMOREPARAMS(nick, command) \
  (std::string(":") + SRV_NAME + " 461 " + nick + " " + command + \
  " :Not enough parameters\r\n")
// A client command cannot be parsed because not enough parameters were supplied.

#define _462_ERR_ALREADYREGISTERED(nick) \
  (std::string(":") + SRV_NAME + " 462 " + nick + " :You may not reregister\r\n")
// Returned when a client tries to change a detail that can only be set during
// registration (such as PASS, USER...)

#define _464_ERR_PASSWD_MISMATCH(nick) \
  (std::string(":") + SRV_NAME + " 464 " + nick + " :Password incorrect\r\n")

#define _482_ERR_CHANOPRIVSNEEDED(nick, chanName) \
  (std::string(":") + SRV_NAME + " 482 " + nick + " "  + chanName + \
  " :You're not a channel operator\r\n")

//------------------------------------------------------------------------------

// _404_ERR_CANNOTSENDTOCHAN: Cannot send to channel. Indicates that the user cannot send messages to the channel, which may happen if the user is banned or if the channel is set to "invite only."

// _405_ERR_TOOMANYCHANNELS: The user has joined too many channels.

// _411_ERR_NORECIPIENT: Returned by the PRIVMSG command to indicate the message wasn’t delivered because there was no recipient given.

// _412_ERR_NOTEXTTOSEND: Returned by the PRIVMSG command to indicate the message wasn’t delivered because there was no text to send.

// _417_ERR_INPUTTOOLONG Returned when a message is too long for the server to process.
// (512 bytes for the main section, 4094 or 8191 bytes for the tag section)

// _441_ERR_USERNOTINCHANNEL: The user is not in the channel.

// _451_ERR_NOTREGISTERED: "<client> :You have not registered"

#define _471_ERR_CHANNELISFULL(nick, chanName) \
  (std::string(":") + SRV_NAME + " 471 " + nick + chanName + \
  " :Cannot join channel (+l)\r\n")
// : The channel has reached its user number limit set with +l mode

#define _473_ERR_INVITEONLYCHAN(nick, chanName) \
  (std::string(":") + SRV_NAME + " 473 " + nick + chanName + \
  " :Cannot join channel (+i)\r\n")
// The channel is invite-only bur the client was not invited.

#define _475_ERR_BADCHANNELKEY(nick, chanName) \
  (std::string(":") + SRV_NAME + " 475 " + nick + chanName + \
  " :Cannot join channel (+k)\r\n")
// wrong channel key(password) provided

#define _476_ERR_BADCHANMASK(nick, chanName) \
  (std::string(":") + SRV_NAME + " 476 " + nick + chanName + \
  " :Bad Channel mask\r\n")
// A client attempted to join a channel with an invalid chennel name format that doesn't much the server's accepted format.

// _472_ERR_UNKNOWNMODE: The mode is unknown.

// _481_ERR_NOPRIVILEGES: You're not an IRC operator"

// _501_ERR_UMODEUNKNOWNFLAG: Unknown MODE flag

// 525_ERR_INVALIDKEY : The key provided for a channel is invalid.

// ?? _465_ERR_YOUREBANNEDCREEP: You are banned from this server.

/*--------- Functions --------------------------------------------------------*/

/* Welcome messages */
void send001Welcome(int fd, std::string const &nick,
                    std::string const &user, std::string const &host);
void send002Yourhost(int fd, const std::string &nick);
void send003Created(int fd, const std::string &nick,
                    const std::string &startTime);
void send104Myinfo(int fd, const std::string &nick);
void send005Isupport(int fd, const std::string &nick, const std::string &toks);
void sendWelcome(int fd, const std::string &nick);

/* User related messages */
void send221Umodeis(int fd, const Client &client);

/* Channel related messages */
void send331Notopic(int fd, const std::string &nick, const Channel &channel);
void send332Topic(int fd, const std::string &nick, const Channel &channel);
void send333Topicwhotime(int fd, const std::string &nick,
                         const Channel &channel);
void send336Invitelist(int fd, const std::string &nick,
                       const std::string &chanName);
void send337Endofinvitelist(int fd, const std::string &nick);
void send341Inviting(int fd, const std::string &nick,
                     const std::string &invitedNick,
                     const std::string &chanName);
void send353Namreply(int fd, const std::string &nick, const Channel &channel);
void send366Endofnames(int fd, const std::string &nick,
                       const std::string &chanName);

/* Error messages */
void send401NoSuchNick(int fd, const std::string &nick,
                       const std::string &targetNick);
void send403NoSuchChannel(int fd, const std::string &nick,
                          const std::string &chanName);
void send421UnknownCommand(int fd, const std::string &nick,
                           const std::string &command);
void send431NoNicknameGiven(int fd, const std::string &nick);
void send432ErroneusNickname(int fd, const std::string &nick);
void send433NickAlreadyInUse(int fd, const std::string &nick);
void send442NotOnChannel(int fd, const std::string &nick,
                         const std::string &chanName);
void send443UserOnChannel(int fd, const std::string &nick,
                          const std::string &invitedNick,
                          const std::string &chanName);
void send461NeedMoreParams(int fd, const std::string &nick,
                           const std::string &command);
void send462AlreadyRegistered(int fd, const std::string &nick);
void send464PasswdMismatch(int fd, const std::string &nick);
void send471ChannelIsFull(int fd, const std::string &nick,
                          const std::string &chanName);
void send473InviteOnlyChan(int fd, const std::string &nick,
                           const std::string &chanName);
void send475BadChannelKey(int fd, const std::string &nick,
                          const std::string &chanName);
void send476BadChanMask(int fd, const std::string &nick,
                        const std::string &chanName);
void send482ChanOPrivsNeeded(int fd, const std::string &nick,
                             const std::string &chanName);

/* Just for test */
void testAllNumericReplies(const std::string &serverStartTime,
                           const Client &target,
                           const std::string &command,
                           const std::string &targetNick);

/*--------- Just for fun -----------------------------------------------------*/

#define _WELCOME(nick) \
  (std::string(":") + SRV_NAME + " NOTICE " + nick + " :\n" + "\
────────────────────────────────────────────────────────────────────\n\
────────────────────────────────────────────────────────────────────\n\
─────── d 888 ─ ,8\"88e ─── 888 ── 888 88e ─── e88'Y88 ──────────────\n\
────── d8 888 ─ 8  888D ── 888 ── 888 888D ─ d888  'Y ──────────────\n\
───── d88 888e ─── 88P ─── 888 ── 888 88\" ─ 88888 ──────────────────\n\
───── \"\"\" 888\" ── d*\" ──── 888 ── 888 b, ─── Y888  ,d ──  " + "/\\_/\\" + " ────\n\
───────── 888 ─ 8888888 ── 888 ── 888 88bb, ─ \"88,d88 ── " + "( o.o )" + " ───\n\
────────────────────────────────────────────────────────  " + "> ^ <" + " ────\n\
───────────────── powered by faboussa, mbernard & yusengok with " + "♥" + " ──" + "\n\r\n" )

// #define CYAN_ "\x03" "11"
// #define MAGENTA_ "\x03" "13"
// #define RESET_ "\x03" "99"

// #define _WELCOME(nick) \
//   (std::string(":") + SRV_NAME + " NOTICE " + nick + " :\n" + CYAN_ + \
// "────────────────────────────────────────────────────────────────────\n" + CYAN_ + \
// "────────────────────────────────────────────────────────────────────\n" + CYAN_ + \
// "─────── d 888 ─ ,8\"88e ─── 888 ── 888 88e ─── e88'Y88 ──────────────\n" + CYAN_ + \
// "────── d8 888 ─ 8  888D ── 888 ── 888 888D ─ d888  'Y ──────────────\n" + CYAN_ + \
// "───── d88 888e ─── 88P ─── 888 ── 888 88\" ─ 88888 ──────────────────\n" + CYAN_ + \
// "───── \"\"\" 888\" ── d*\" ──── 888 ── 888 b, ─── Y888  ,d ──  " + RESET_ + "/\\_/\\" + CYAN_ + " ────\n" + CYAN_ + \
// "───────── 888 ─ 8888888 ── 888 ── 888 88bb, ─ \"88,d88 ── " + RESET_ + "( o.o )" + CYAN_ + " ───\n" + CYAN_ + \
// "────────────────────────────────────────────────────────  " + RESET_ + "> ^ <" + CYAN_ + " ────\n" + CYAN_ + \
// "───────────────── powered by faboussa, mbernard & yusengok with " + MAGENTA_ + "♥" + CYAN_ + " ──" + RESET_ + "\n\r\n" )

#endif  // INCLUDES_NUMERICREPLIES_HPP_

//  /\_/\     /\_/\     /\_/\
// ( o.o )   ( o.o )   ( o.o )
//  > ^ <     > ^ <     > ^ <
