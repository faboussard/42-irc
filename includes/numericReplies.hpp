/* Copyright 2024 <yusengok>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:37:02 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/30 20:31:01 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_NUMERICREPLIES_HPP_
#define INCLUDES_NUMERICREPLIES_HPP_

#include <string>

#include "./Channel.hpp"
#include "./Client.hpp"
#include "./serverConfig.hpp"

#define FROM_SERVER std::string(":") + SRV_NAME + " "
#define RUNTIME_ERROR "Failed to send numeric reply"

/* Messages definition */

/*------ Connection registration ---------------------------------------------*/

#define _001_RPL_WELCOME(nick, user, host)                            \
  (FROM_SERVER + "001 " + nick + " :Welcome to the " + NETWORK_NAME + \
   " Network, " + nick + "!" + user + "@" + host + "\r\n")

#define _002_RPL_YOURHOST(nick)                                 \
  (FROM_SERVER + "002 " + nick + " :Your host is " + SRV_NAME + \
   ", running version " + SRV_VERSION + "\r\n")

#define _003_RPL_CREATED(nick, starttime)                                      \
  (FROM_SERVER + "003 " + nick + " :This server was created on " + starttime + \
   "\r\n")

#define _004_RPL_MYINFO(nick)                                                \
  (FROM_SERVER + "004 " + nick + " :" + SRV_NAME + " " + SRV_VERSION + " " + \
   USER_MODES + " " + CHANNEL_MODES + "\r\n")

#define _005_RPL_ISUPPORT(nick, tokens)          \
  (FROM_SERVER + "005 " + nick + " :" + tokens + \
   " :are supported by this server\r\n")
// Inform clients about the server-supported features and settings

/*------ User related messages -----------------------------------------------*/

#define _221_RPL_UMODEIS(nick, modes) \
  (FROM_SERVER + "221 " + nick + " " + modes + "\r\n")
// Informs client's currently-set user modes. (Reply to MODE <nick> command)

/*------ Channel related messages --------------------------------------------*/

#define _321_RPL_LISTSTART(nick) \
  (FROM_SERVER + "321 " + nick + " Channel :Users Name\r\n")

#define _322_RPL_LIST(nick, chanName, numUsers, topic)                    \
  (FROM_SERVER + "322 " + nick + " " + chanName + " " + numUsers + " :" + \
   topic + "\r\n")

#define _323_RPL_LISTEND(nick) \
  (FROM_SERVER + "323 " + nick + " :End of /LIST\r\n")

#define _324_RPL_CHANNELMODEIS(nick, chanName, modeFlag, modeArgs)       \
  (FROM_SERVER + "324 " + nick + " " + chanName + " " + modeFlag + " " + \
   modeArgs + "\r\n")
// Channel mode is displayed with arguments associated with the modes.
// Reply to MODE <prefix><channel>

#define _329_RPL_CREATIONTIME(nick, chanName, creationTime) \
  (FROM_SERVER + "329 " + nick + " " + chanName + " " + creationTime + "\r\n")
// creation time of a channel. Reply to MODE <prefix><channel>

#define _331_RPL_NOTOPIC(nick, chanName) \
  (FROM_SERVER + "331 " + nick + " " + chanName + " :No topic is set\r\n")
// Indicates that the channel has no topic set.

#define _332_RPL_TOPIC(nick, chanName, topic) \
  (FROM_SERVER + "332 " + nick + " " + chanName + " :" + topic + "\r\n")
// Current topic of the channel.

#define _333_RPL_TOPICWHOTIME(nick, chanName, author, setTime)         \
  (FROM_SERVER + "333 " + nick + " " + chanName + " " + author + " " + \
   setTime + "\r\n")
// By whom and when(unix timestamp) is the topic set. Sent with 332

#define _336_RPL_INVITELIST(nick, chanName) \
  (FROM_SERVER + "336 " + nick + " " + chanName + "\r\n")
// Indicate a channel the client was invited to.

#define _337_RPL_ENDOFINVITELIST(nick) \
  (FROM_SERVER + "337 " + nick + " :End of /INVITE list\r\n")

#define _341_RPL_INVITING(nick, invitedNick, chanName) \
  (FROM_SERVER + "341 " + nick + +" " + invitedNick + " " + chanName + "\r\n")
// Confirms the user has been invited to the channel.
// Sent as a reply to the INVITE command.

#define _353_RPL_NAMREPLY(nick, chanNameWithSymbol, nicknames)                 \
  (FROM_SERVER + "353 " + nick + " " + chanNameWithSymbol + " :" + nicknames + \
   "\r\n")
// : List of users in the channel.

#define _366_RPL_ENDOFNAMES(nick, chanName) \
  (FROM_SERVER + "366 " + nick + " " + chanName + " :End of /NAMES list\r\n")

/*------ Error messages ------------------------------------------------------*/

#define _401_ERR_NOSUCHNICK(nick, target) \
  (FROM_SERVER + "401 " + nick + " " + target + " :No such nick/channel\r\n")
// Indicates that no client can be found for the supplied nickname.

#define _403_ERR_NOSUCHCHANNEL(nick, chanName) \
  (FROM_SERVER + "403 " + nick + " " + chanName + " :No such channel\r\n")

#define _404_ERR_CANNOTSENDTOCHAN(nick, chanName) \
  (FROM_SERVER + "404 " + nick + " " + chanName + \
   " :Cannot send to channel\r\n")
// Indicates that the user cannot send messages to the channel,
// which may happen if the channel is set to "invite only."

#define _405_ERR_TOOMANYCHANNELS(nick) \
  (FROM_SERVER + "405 " + nick + " :You have joined too many channels\r\n")

#define _409_ERR_NOORIGIN(nick) \
  (FROM_SERVER + "409 " + nick + " :No origin specified\r\n")

#define _411_ERR_NORECIPIENT(nick, command) \
  (FROM_SERVER + "411 " + nick + " :No recipient given(" + command + ")\r\n")
// Client tries to send a command that requires a recipient
// (such as PRIVMSG or NOTICE) but does not specify one.

#define _412_ERR_NOTEXTTOSEND(nick) \
  (FROM_SERVER + "412 " + nick + " :No text to send\r\n")
// Client issues a command like PRIVMSG or NOTICE
// but does not include any text in the message body.

#define _417_ERR_INPUTTOOLONG(nick) \
  (FROM_SERVER + "417 " + nick + " :Input line was too long\r\n")
//  Returned when a message is too long for the server to process.
// (512 bytes for the main section, 4094 or 8191 bytes for the tag section)

#define _421_ERR_UNKNOWNCOMMAND(nick, command) \
  (FROM_SERVER + "421 " + nick + " " + command + " :Unknown command\r\n")

#define _431_ERR_NONICKNAMEGIVEN(nick) \
  (FROM_SERVER + "431 " + nick + " :No nickname given\r\n")
// Returned when a nickname parameter is expected for a command but isn’t given.

#define _432_ERR_ERRONEUSNICKNAME(nick) \
  (FROM_SERVER + "432 " + nick + " :Erroneus nickname\r\n")
// The desired nickname contains characters that are disallowed by the server.

#define _433_ERR_NICKNAMEINUSE(nick) \
  (FROM_SERVER + "433 " + nick + " :Nickname is already in use\r\n")

#define _442_ERR_NOTONCHANNEL(nick, chanName)     \
  (FROM_SERVER + "442 " + nick + " " + chanName + \
   " :You're not on that channel\r\n")

#define _443_ERR_USERONCHANNEL(nick, invitedNick, chanName)           \
  (FROM_SERVER + "443 " + nick + " " + invitedNick + " " + chanName + \
   " :is already on channel\r\n")

#define _441_ERR_USERNOTINCHANNEL(nick, targetNick, chanName)        \
  (FROM_SERVER + "441 " + nick + " " + targetNick + " " + chanName + \
   " :They aren't on that channel\r\n")
// Returned when a client tries to perform a channel+nick affecting command,
// when the nick isn’t joined to the channel (e.g. MODE #channel +o nick).

#define _451_ERR_NOTREGISTERED(nick) \
  (FROM_SERVER + "451 " + nick + " :You have not registered\r\n")
// Returned when a client tries to send a message before registering.

#define _461_ERR_NEEDMOREPARAMS(nick, command) \
  (FROM_SERVER + "461 " + nick + " " + command + " :Not enough parameters\r\n")
// Command cannot be parsed because not enough parameters were supplied.

#define _462_ERR_ALREADYREGISTERED(nick) \
  (FROM_SERVER + "462 " + nick + " :You may not reregister\r\n")
// Returned when a client tries to change a detail that can only be set during
// registration (such as PASS, USER...)

#define _464_ERR_PASSWD_MISMATCH(nick) \
  (FROM_SERVER + "464 " + nick + " :Password incorrect\r\n")

#define _471_ERR_CHANNELISFULL(nick, chanName)    \
  (FROM_SERVER + "471 " + nick + " " + chanName + \
   " :Cannot join channel (+l)\r\n")
// : The channel has reached its user number limit set with +l mode

#define _472_ERR_UNKNOWNMODE(nick, modechar)      \
  (FROM_SERVER + "472 " + nick + " " + modechar + \
   " :is unknown mode char to me\r\n")

#define _473_ERR_INVITEONLYCHAN(nick, chanName)   \
  (FROM_SERVER + "473 " + nick + " " + chanName + \
   " :Cannot join channel (+i)\r\n")
// The channel is invite-only bur the client was not invited.

#define _475_ERR_BADCHANNELKEY(nick, chanName)    \
  (FROM_SERVER + "475 " + nick + " " + chanName + \
   " :Cannot join channel (+k)\r\n")
// wrong channel key(password) provided

#define _476_ERR_BADCHANMASK(nick, chanName) \
  (FROM_SERVER + "476 " + nick + " " + chanName + " :Bad Channel mask\r\n")
// A client attempted to join a channel with an invalid chennel name format
// that doesn't much the server's accepted format.

#define _481_ERR_NOPRIVILEGES(nick) \
  (FROM_SERVER + "481 " + nick +    \
   " :Permission Denied- You're not an IRC operator\r\n")

#define _482_ERR_CHANOPRIVSNEEDED(nick, chanName) \
  (FROM_SERVER + "482 " + nick + " " + chanName + \
   " :You're not a channel operator\r\n")

#define _501_ERR_UMODEUNKNOWNFLAG(nick) \
  (FROM_SERVER + "501 " + nick + " :Unknown MODE flag\r\n")

#define _525_ERR_INVALIDKEY(nick, chanName)       \
  (FROM_SERVER + "525 " + nick + " " + chanName + \
   " :Key is not well-formed\r\n")

/* Functions */

/*------ Welcome messages ----------------------------------------------------*/

void send001Welcome(int fd, std::string const &nick, std::string const &user,
                    std::string const &host);
void send002Yourhost(int fd, const std::string &nick);
void send003Created(int fd, const std::string &nick,
                    const std::string &startTime);
void send104Myinfo(int fd, const std::string &nick);
void send005Isupport(int fd, const std::string &nick, const std::string &toks);
void sendWelcome(int fd, const std::string &nick);

/*------ User related replies ------------------------------------------------*/

void send221Umodeis(int fd, const Client &client);

/*------ Channel related replies ---------------------------------------------*/

void send321Liststart(int fd, const std::string &nick);
void send322List(int fd, const std::string &nick, const Channel &channel);
void send323Listend(int fd, const std::string &nick);
void send324Channelmodeis(int fd, const std::string &nick,
                          const Channel &channel);
void send329Creationtime(int fd, const std::string &nick,
                         const Channel &channel);
void send331Notopic(int fd, const std::string &nick, const Channel &channel);
void send332Topic(int fd, const std::string &nick, const Channel &channel);
void send333Topicwhotime(int fd, const std::string &nick,
                         const Channel &channel);
void send336Invitelist(int fd, const std::string &nick,
                       const Channel &channel);
void send337Endofinvitelist(int fd, const std::string &nick);
void send341Inviting(int fd, const std::string &nick,
                     const std::string &invitedNick,
                     const Channel &channel);
void send353Namreply(int fd, const std::string &nick, const Channel &channel);
void send366Endofnames(int fd, const std::string &nick,
                       const Channel &channel);

/*------ Error messages ------------------------------------------------------*/

void send401NoSuchNick(int fd, const std::string &nick,
                       const std::string &targetNick);
void send403NoSuchChannel(int fd, const std::string &nick,
                          const std::string &chanName);
void send404CannotSendToChan(int fd, const std::string &nick,
                             const Channel &channel);
void send405TooManyChannels(int fd, const std::string &nick);
void send409NoOrigin(int fd, const std::string &nick);
void send411NoRecipient(int fd, const std::string &nick,
                        const std::string &command);
void send412NoTextToSend(int fd, const std::string &nick);
void send417InputTooLong(int fd, const std::string &nick);
void send421UnknownCommand(int fd, const std::string &nick,
                           const std::string &command);
void send431NoNicknameGiven(int fd, const std::string &nick);
void send432ErroneusNickname(int fd, const std::string &nick);
void send433NickAlreadyInUse(int fd, const std::string &nick);
void send441UserNotInChannel(int fd, const std::string &nick,
                             const std::string &targetNick,
                             const Channel &channel);
void send442NotOnChannel(int fd, const std::string &nick,
                         const Channel &channel);
void send443UserOnChannel(int fd, const std::string &nick,
                          const std::string &invitedNick,
                          const Channel &channel);
void send451NotRegistered(int fd, const std::string &nick);
void send461NeedMoreParams(int fd, const std::string &nick,
                           const std::string &command);
void send462AlreadyRegistered(int fd, const std::string &nick);
void send464PasswdMismatch(int fd, const std::string &nick);
void send471ChannelIsFull(int fd, const std::string &nick,
                          const Channel &channel);
void send472UnknownMode(int fd, const std::string &nick,
                        const std::string &modeChar);
void send473InviteOnlyChan(int fd, const std::string &nick,
                           const Channel &channel);
void send475BadChannelKey(int fd, const std::string &nick,
                          const Channel &channel);
void send476BadChanMask(int fd, const std::string &nick,
                        const Channel &channel);
void send481NoPrivileges(int fd, const std::string &nick);
void send482ChanOPrivsNeeded(int fd, const std::string &nick,
                             const Channel &channel);
void send501UmodeUnknownFlag(int fd, const std::string &nick);
void send525InvalidKey(int fd, const std::string &nick,
                       const Channel &channel);

/*--------- Test -------------------------------------------------------------*/

void testAllNumericReplies(const std::string &serverStartTime,
                           const Client &client, const std::string &command,
                           const std::string &targetNick);

/*--------- Just for fun -----------------------------------------------------*/

#define _WELCOME(nick)                                                         \
  (std::string(":") + SRV_NAME + " NOTICE " + nick + " :\n" +                  \
   "────────────────────────────────────────────────────────────────────\n" +  \
   "────────────────────────────────────────────────────────────────────\n" +  \
   "─────── d 888 ─ ,8\"88e ─── 888 ── 888 88e ─── e88'Y88 ──────────────\n" + \
   "────── d8 888 ─ 8  888D ── 888 ── 888 888D ─ d888  'Y ──────────────\n" +  \
   "───── d88 888e ─── 88P ─── 888 ── 888 88\" ─ 88888 ──────────────────\n" + \
   "───── \"\"\" 888\" ── d*\" ──── 888 ── 888 b, ─── Y888  ,d ──  " +         \
   "/\\_/\\" + " ────\n" +                                                     \
   "───────── 888 ─ 8888888 ── 888 ── 888 88bb, ─ \"88,d88 ── " + "( o.o )" +  \
   " ───\n" + "────────────────────────────────────────────────────────  " +   \
   "> ^ <" + " ────\n" +                                                       \
   "───────────────── powered by faboussa, mbernard & yusengok with " + "♥" +  \
   " ──" + "\n\r\n")

#endif  // INCLUDES_NUMERICREPLIES_HPP_

//  /\_/\     /\_/\     /\_/\
// ( o.o )   ( o.o )   ( o.o )
//  > ^ <     > ^ <     > ^ <
