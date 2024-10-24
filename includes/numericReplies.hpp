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

#define SERVER_NAME "ircserv"

// #define _101_RPL_WELCOME(nick, user, host) ":" + SERVER_NAME + " " + numeric + \ 
//         " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host

#define _332_RPL_TOPIC ":The topic of a channel."

ssize_t sendNumericReply(const std::string& numeric, const Client& target,
                         const std::string& message);
void send101(const Client& target);


#endif  // NUMERIC_REPLIES_HPP_