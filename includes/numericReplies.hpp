/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:37:02 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/18 09:24:42 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_NUMERICREPLIES_HPP_
#define INCLUDES_NUMERICREPLIES_HPP_

#include <string>

#include "./Channel.hpp"
#include "./Client.hpp"
#include "./Config.hpp"
#include "./Server.hpp"
#include "./utils.hpp"

extern Config *gConfig;

#define FROM_SERVER (std::string(":") + SRV_NAME + " ")
#define RUNTIME_ERROR "Failed to send numeric reply"
#define HERE "H"  // Our server doesn't support G(Gone) status
#define HOPCOUNT "0"

/* Messages definition */

/*------ Connection registration ---------------------------------------------*/

#define _001_RPL_WELCOME(nick, user, host)                                 \
  (FROM_SERVER + "001 " + nick + " :Welcome to the " +                     \
  gConfig->getParam(NETWORK) + " Network, " + nick + "!" + user + "@" +  \
  host + "\r\n")

#define _002_RPL_YOURHOST(nick)                                 \
  (FROM_SERVER + "002 " + nick + " :Your host is " + SRV_NAME + \
  ", running version " + SRV_VERSION + "\r\n")

#define _003_RPL_CREATED(nick, starttime)                                      \
  (FROM_SERVER + "003 " + nick + " :This server was created on " + starttime + \
  "\r\n")

#define _004_RPL_MYINFO(nick)                                                 \
  (FROM_SERVER + "004 " + nick + " :" + SRV_NAME + " " + SRV_VERSION + " " +  \
  gConfig->getParam(USERMODES) + " " + gConfig->getSupportedChanModes() +   \
  "\r\n")

#define _005_RPL_ISUPPORT(nick)                                            \
  (FROM_SERVER + "005 " + nick + " :" + gConfig->getIsupportParamToken() + \
  ":are supported by this server\r\n")
// Inform clients about the server-supported features and settings

/*------ User related messages -----------------------------------------------*/

#define _352_RPL_WHOREPLY(targetNick, chanName, userN, hostN, nick, realN)     \
  (FROM_SERVER + "352 " + targetNick + " " + chanName + " " + userN + " " +    \
  hostN + " " + SRV_NAME +  " " + nick + " " + HERE + " :" + HOPCOUNT + " " +  \
  realN + "\r\n")

#define _315_RPL_ENDOFWHO(nick, chanName) \
  (FROM_SERVER + "315 " + nick + " " + chanName + " :End of /WHO list\r\n")

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
  (FROM_SERVER + "366 " + nick + " " + chanName + " :End of \\NAMES list\r\n")

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

#define _407_ERR_TOOMANYTARGETS(nick) \
  (FROM_SERVER + "407 " + nick + " :Too many targets\r\n")

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

#define _432_ERR_ERRONEUSNICKNAME(nick, erroneusNick) \
  (FROM_SERVER + "432 " + nick + " " + erroneusNick + " :Erroneus nickname\r\n")
// The desired nickname contains characters that are disallowed by the server.

#define _433_ERR_NICKNAMEINUSE(nick, nickInUse)    \
  (FROM_SERVER + "433 " + nick + " " + nickInUse + \
  " :Nickname is already in use\r\n")

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

void sendNumericReply(int fd, std::string *message);

/*------ Welcome messages ----------------------------------------------------*/

void send001Welcome(int fd, std::string const &nick, std::string const &user,
                    std::string const &host);
void send002Yourhost(int fd, std::string const &nick);
void send003Created(int fd, std::string const &nick,
                    const std::string &startTime);
void send104Myinfo(int fd, std::string const &nick);
void send005Isupport(int fd, std::string const &nick);
void sendWelcome(int fd, std::string const &nick);

/*------ User related replies ------------------------------------------------*/

void send315EndOfWho(const Client &client, const Channel &channel);
void send352Whoreply(const Client &client, const Client &clientInChannel, \
                     const Channel &channel);

/*------ Channel related replies ---------------------------------------------*/

void send321Liststart(int fd, const std::string &nick);
void send322List(int fd, const std::string &nick, const Channel &channel);
void send323Listend(int fd, const std::string &nick);
void send324Channelmodeis(const Client &client, const Channel &channel);
void send329Creationtime(const Client &client, const Channel &channel);
void send331Notopic(const Client &client, const Channel &channel);
void send332Topic(const Client &client, const Channel &channel);
void send333Topicwhotime(const Client &client, const Channel &channel);
void send336Invitelist(const Client &client, const Channel &channel);
void send337Endofinvitelist(const Client &client);
void send341Inviting(const Client &client, const std::string &invitedNick,
                     const Channel &channel);
void send353Namreply(const Client &client, const Channel &channel);
void send366Endofnames(const Client &client, const Channel &channel);

/*------ Error messages ------------------------------------------------------*/

void send401NoSuchNick(const Client &client, const std::string &targetNick);
void send403NoSuchChannel(const Client &client, const std::string &chanName);
void send404CannotSendToChan(const Client &client, const Channel &channel);
void send405TooManyChannels(const Client &client);
void send407TooManyTargets(const Client &client);
void send409NoOrigin(const Client &client);
void send411NoRecipient(const Client &client, const std::string &command);
void send412NoTextToSend(const Client &client);
void send417InputTooLong(const Client &client);
void send421UnknownCommand(const Client &client, const std::string &command);
void send431NoNicknameGiven(const Client &client);
void send432ErroneusNickname(const Client &client,
                             const std::string &erroneusNick);
void send433NickAlreadyInUse(const Client &client,
                             const std::string &nickInUse);
void send441UserNotInChannel(const Client &client,
                             const std::string &targetNick,
                             const Channel &channel);
void send442NotOnChannel(const Client &client, const Channel &channel);
void send443UserOnChannel(const Client &client, const std::string &invitedNick,
                          const Channel &channel);
void send451NotRegistered(const Client &client);
void send461NeedMoreParams(const Client &client, const std::string &command);
void send462AlreadyRegistered(const Client &client);
void send464PasswdMismatch(const Client &client);
void send471ChannelIsFull(const Client &client, const Channel &channel);
void send472UnknownMode(const Client &client, const std::string &modeChar);
void send473InviteOnlyChan(const Client &client, const Channel &channel);
void send475BadChannelKey(const Client &client, const Channel &channel);
void send476BadChanMask(const Client &client,
                        const std::string &chanNameWithBadMask);
void send481NoPrivileges(const Client &client);
void send482ChanOPrivsNeeded(const Client &client, const Channel &channel);
void send501UmodeUnknownFlag(const Client &client);
void send525InvalidKey(const Client &client, const Channel &channel);

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
