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
// source: The source of the message (server name or a user's nickname for direct user messages).
// target: nickname of the client receiving the message
// parameters: Additional parameters depending on the type of message
// message: A human readable text message (welcome message or error description)
// Example of format:
// :irc.com 001 <nick> :Welcome to the Internet Relay Network <nick>!<user>@<host>

#define RPL_WELCOME 001


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


// PING: Request a PONG response from a server or client to check if it is still active