/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 15:09:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/18 11:53:00 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "../../includes/Server.hpp"

// ======== PING <server or timestamp> =========================================
// Replies:
// PONG <server or timestamp>

// ======== PING (without parameters) ==========================================
// -- 409 ERR_NOORIGIN

// Hexchat autonmatically adds timestamp when we send /PING
// netcat doesn't add anything with /PING

void Server::ping(const Client &client, const std::string &token) {
  if (token.empty()) {
    send409NoOrigin(client);
    return;
  }
  {
    std::ostringstream oss;
    oss << client.getNickname() << "sent PING " << token;
    printLog(INFO_LOG, CLIENT, oss.str());
    std::string message = "PONG " + token + "\r\n";
    client.receiveMessage(message);
  }
  {
    std::ostringstream oss;
    oss << "PONG " << token << " sent to " << client.getNickname();
    printLog(INFO_LOG, REPLY, oss.str());
  }
}
