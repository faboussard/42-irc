/* Copyright 2024 <yusengok>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 13:59:30 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/23 14:08:10 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ssize_t send(int sockfd, const void *buf, size_t len, int flags);

// general format of how numeric replies are structured when sent to the client:
// :<source> <numeric> <target> <parameters> :<message>
// source: The source of the message (server name or a user's nickname for
// direct user messages). target: nickname of the client receiving the message
// parameters: Additional parameters depending on the type of message
// message: A human readable text message (welcome message or error description)
// Example of format:
// :irc.com 001 <nick> :Welcome to the Internet Relay Network <nick>!<user>@<host>

//==== On terminal ====
//--Hexchat----------------
// New client connected: 4
// Received message from client 4: CAP LS 302
// PASS pass

// Received message from client 4: NICK yusengok
// USER yusengok 0 * :realname

//--netcat-----------------
//New client connected: 5


#include "../includes/numericReplies.hpp"

#include <sys/socket.h>

#include <string>

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"

// ssize_t sendNumericReply(const std::string& numeric, const Client& target,
//                          const std::string& message) {
//   std::string numericReply = ':' + SERVER_NAME + ' ' + numeric + ' ' + message;
//   if (send(target.getFd(), numericReply.c_str(), numericReply.size(), 0) ==
//       -1) {
//     throw std::runtime_error("Failed to send numeric reply");
//   }
// }

// #define _101_RPL_WELCOME ":Welcome to the Internet Relay Network !"

// void send101(const Client& target) {
//   std::string nick = target.getNickName().empty() ? "*" : target.getNickName();
//   std::string user = target.getUserName().empty() ? "*" : target.getUserName();
//   std::string host = target.getIp().empty() ? "*" : target.getIp();  // from server
//   std::string message =
//       nick + _101_RPL_WELCOME + nick + "!" + user + "@" + host;
//   sendNumericReply("101", target, message);
// }

#define _101_RPL_WELCOME(nick, user, host) (":" + SERVER_NAME + " 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host)

void send101(const Client& target) {
  std::string nick = target.getNickName().empty() ? "*" : target.getNickName();
  std::string user = target.getUserName().empty() ? "*" : target.getUserName();
  std::string host = target.getIp().empty() ? "*" : target.getIp();  // from server
  std::string message = _101_RPL_WELCOME(nick.c_str(), user.c_str(), host.c_str());
  if (send(target.getFd(), message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error("Failed to send numeric reply");
  }
}

// ":serverName 001 userNickname : PRL_WELCOME"

// JOIN: Add a client to a channel
// 	- 332 RPL_TOPIC: The topic of a channel.

// KICK: Remove a client from a channel

// INVITE: Invite a client to a channel

// TOPIC: Change or view the channel topic

// MODE: Change the channel’s mode

// LIST: Show a list of available channels, often filtered by parameters

// NOTICE: Send a notice message to a user or a channel

// NICK: Change the client’s nickname
//   - 433: ERR_NICKNAMEINUSE (Nickname is already in use)

// PRIVMSG: Send a private message to a user or a channel
//   - 401: ERR_NOSUCHNICK (No such nickname exists)

// QUIT: Disconnect from the server

// PING: Request a PONG response from a server or client to check if it is still
// active