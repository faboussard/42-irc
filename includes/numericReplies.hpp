/* Copyright 2024 <yusengok>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:37:02 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/24 09:37:51 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NUMERIC_REPLIES_HPP_
#define NUMERIC_REPLIES_HPP_

#include "./Client.hpp"

#define SRV_NAME "ircserv"
#define SRV_VERSION "1.0.0"
#define NETWORK_NAME "42 IRC"
#define USER_MODES "ao"       // Temporary definition
#define CHANNEL_MODES "mtov"  // Temporary definition

#define RUNTIME_ERROR "Failed to send numeric reply"

/* Message definition */
#define _101_RPL_WELCOME(nick, user, host)                              \
  (std::string(":") + SRV_NAME + " 001 " + nick + " :Welcome to the " + \
   NETWORK_NAME + " Network, " + nick + "!" + user + "@" + host + "\r\n")

#define _102_RPL_YOURHOST(nick)                                       \
  (std::string(":") + SRV_NAME + " 002 " + nick + " :Your host is " + \
   SRV_NAME + ", running version " + SRV_VERSION + "\r\n")

#define _103_RPL_CREATED(nick, starttime)         \
  (std::string(":") + SRV_NAME + " 003 " + nick + \
   " :This server was created on " + starttime + "\r\n")

#define _104_RPL_MYINFO(nick)                                            \
  (std::string(":") + SRV_NAME + " 004 " + nick + " " + SRV_NAME + " " + \
   SRV_VERSION + " " + USER_MODES + " " + CHANNEL_MODES + "\r\n")
//----- The available user modes:
// +i	Invisible: The user is hidden from /WHO queries unless they are in the
// same channel. +o	Operator: Grants IRC operator privileges to the user. +w
// Wallops: The user receives WALLOPS messages, which are special notices sent
// by IRC operators. +s	Server Notices: The user receives server notices from
// the IRC server. +r	Registered: The user has registered with services or
// identified themselves. +x	Hostmasking: Hides the user's IP address or
// hostname from other users. +a	Administrator: Grants server
// administrator privileges. +g	Caller ID: Blocks private messages from users
// who are not on the user's allow list.
//----- The available channel modes:
// +i	Invite-only: Only invited users can join the channel.
// +m	Moderated: Only users with voice (+v) or operator (+o) privileges can
// send messages. +n	No external messages: Prevents users outside the channel
// from sending messages into the channel. +p	Private: The channel does not
// appear in channel lists (/LIST), but can be joined if the name is known. +s
// Secret: The channel is secret, meaning it doesn't appear in /WHOIS results or
// in channel lists. +t	Topic settable by ops only: Only channel operators can
// change the topic. +k	Key: A password is required to join the channel. +l
// Limit: Limits the number of users that can join the channel. +v	Voice:
// Allows a user to speak in a moderated channel. +o	Operator: Grants
// operator privileges within the channel, allowing the user to manage the
// channel (kick users, change modes, etc.).

/* Error messages */

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

//----- JOIN: Add a client to a channel
// 353 RPL_NAMREPLY: List of users in the channel.
// 366 RPL_ENDOFNAMES: End of the list of users in the channel.
// 475 ERR_BADCHANNELKEY: Cannot join the channel (wrong channel key).

// KICK: Remove a client from a channel
// 403 ERR_NOSUCHCHANNEL: Channel does not exist.
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
// 403 ERR_NOSUCHCHANNEL: Channel does not exist.
// 472 ERR_UNKNOWNMODE: The mode is unknown.

// LIST: Show a list of available channels, often filtered by parameters
// 322 RPL_LIST: Channel list with details.
// 323 RPL_LISTEND: End of channel list.

// NOTICE: Send a notice message to a user or a channel
// 401 ERR_NOSUCHNICK: Target nickname or channel does not exist.

// PRIVMSG: Send a private message to a user or a channel
//   - 401: ERR_NOSUCHNICK (No such nickname exists)
// 401 ERR_NOSUCHNICK: No such nickname or channel.
// 402 ERR_NOSUCHSERVER: No such server.
// 404 ERR_CANNOTSENDTOCHAN: Cannot send to that channel.

/* Welcome messages */
void send101Welcome(std::string const& nick, std::string const& user,
                    std::string const& host, int fd);
void send102Yourhost(std::string const& nick, int fd);
void send103Created(const std::string& nick, const std::string& startTime,
                    int fd);
void send104Myinfo(std::string const& nick, int fd);

/* Error messages */
void send433NickAlreadyInUse(const std::string& nick, int fd);
void send464PasswdMismatch(std::string const& nick, int fd);

#endif  // NUMERIC_REPLIES_HPP_