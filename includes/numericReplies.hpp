/* Copyright 2024 <yusengok>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:37:02 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/25 08:35:06 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_NUMERICREPLIES_HPP_
#define INCLUDES_NUMERICREPLIES_HPP_

#include <string>

#include "./Client.hpp"
#include "./serverConfig.hpp"

#define RUNTIME_ERROR "Failed to send numeric reply"

/* Message definition */
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
// +O   Local Operator : has operator privileges for their server, and not for the rest of the network.
// +r   Registered: The user has registered with services or identified themselves.
// +w   Wallops: The user receives WALLOPS messages, which are special notices sent by IRC operators.
//----- The available channel modes -------------------
// +i   Invite-only: Only invited users can join the channel.
// +t   Topic settable by ops only: Only channel operators can change the topic.
// +k   Key: A password is required to join the channel.
// +o   Operator:
// +l   Limit: Limits the number of users that can join the channel.

#define _005_RPL_ISUPPORT(nick, tokens) \
  (std::string(":") + SRV_NAME + " 005 " + nick + " :" + tokens + \
  " :are supported by this server\r\n")
// informs clients about the server-supported features and settings,
// with tokens that can be added, modified, or negated to reflect server changes,
// while the 105 provides similar information from external servers without altering client behavior.

/* Error messages */

#define _421_ERR_UNKNOWNCOMMAND(nick, command) \
  (std::string(":") + SRV_NAME + " 421 " + nick + " :" + command + \
  " :Unknown command\r\n")

// NICK: Change the client’s nickname
// 431 ERR_NONICKNAMEGIVEN:   "<client> :No nickname given"
// Returned when a nickname parameter is expected for a command but isn’t given.

// 432 ERR_ERRONEUSNICKNAME: Invalid nickname format. "<client> <nick> :Erroneus
// nickname"
// Returned when a NICK command cannot be successfully completed as the desired
// nickname contains characters that are disallowed by the server. See the NICK
// command for more information on characters which are allowed in various IRC
// servers. The text used in the last param of this message may vary.

#define _433_ERR_NICKNAMEINUSE(nick)              \
  (std::string(":") + SRV_NAME + " 433 " + nick + \
  " :Nickname is already in use\r\n")

// PASS
#define _464_ERR_PASSWD_MISMATCH(nick) \
  (std::string(":") + SRV_NAME + " 464 " + nick + " :Password incorrect\r\n")

// USER
// 461 ERR_NEEDMOREPARAMS:   "<client> <command> :Not enough parameters"
// Returned when a client command cannot be parsed because not enough parameters
// were supplied. The text used in the last param of this message may vary.

// 462 ERR_ALREADYREGISTERED:  "<client> :You may not reregister"
// Returned when a client tries to change a detail that can only be set during
// registration (such as resending the PASS or USER after registration). The
// text used in the last param of this message varies.

// OPER
// RPL_YOUREOPER (381)
// ERR_NOOPERHOST (491)

// JOIN
// 331 RPL_NOTOPIC - Indicates that the channel has no topic set.
// 353 RPL_NAMREPLY: List of users in the channel.
// 366 RPL_ENDOFNAMES: End of the list of users in the channel.
// 475 ERR_BADCHANNELKEY: Cannot join the channel (wrong channel key).
// 403 ERR_NOSUCHCHANNEL: Channel does not exist.
// 404 ERR_CANNOTSENDTOCHAN: Cannot send to channel. Indicates that the user cannot send messages to the channel, which may happen if the user is banned or if the channel is set to "invite only."
// 405 ERR_TOOMANYCHANNELS: The user has joined too many channels.
// 474 ERR_BANNEDFROMCHAN: The user is banned from the channel.

// KICK: Remove a client from a channel
// 441 ERR_USERNOTINCHANNEL: The user is not in the channel.
// 482 ERR_CHANOPRIVSNEEDED: You’re not a channel operator.

// INVITE: Invite a client to a channel
// 341 RPL_INVITING: Confirms the user has been invited to the channel.
// 443 ERR_USERONCHANNEL: The user is already in the channel.

// TOPIC: Change or view the channel topic
// 332 RPL_TOPIC: Current topic of the channel.
// 482 ERR_CHANOPRIVSNEEDED: You're not a channel operator.

// MODE: Change the channel’s mode
// 324 RPL_CHANNELMODEIS: Channel mode is displayed.
// 472 ERR_UNKNOWNMODE: The mode is unknown.

// LIST: Show a list of available channels, often filtered by parameters
// 322 RPL_LIST: Channel list with details.
// 323 RPL_LISTEND: End of channel list.

// PRIVMSG: Send a private message to a user or a channel
// 401 ERR_NOSUCHNICK: No such nickname or channel.
// 404 ERR_CANNOTSENDTOCHAN: Cannot send to that channel.
// 300 RPL_WHOISUSER: Details about the user being queried.

/* Welcome messages */
void send001Welcome(int fd, std::string const &nick,
                    std::string const &user, std::string const &host);
void send002Yourhost(int fd, const std::string &nick);
void send003Created(int fd, const std::string &nick,
                    const std::string &startTime);
void send104Myinfo(int fd, const std::string &nick);
void send005Isupport(int fd, const std::string &nick, const std::string &toks);

/* Error messages */
void send421UnknownCommand(int fd, const std::string &nick,
                           const std::string &command);
void send433NickAlreadyInUse(int fd, const std::string &nick);
void send464PasswdMismatch(int fd, const std::string &nick);

#endif  // INCLUDES_NUMERICREPLIES_HPP_
