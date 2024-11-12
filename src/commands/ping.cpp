/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 15:09:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/12 11:53:55 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

// ======== PING <server or timestamp> =========================================
// Replies:
// PONG <server or timestamp>

// ======== PING (without parameters) ==========================================
// -- 409 ERR_NOORIGIN

// Hexchat autonmatically adds timestamp when we send /PING
// netcat doesn't add anything with /PING

void Server::ping(Client *client, const std::string &token) {
  if (token.empty()) {
    send409NoOrigin(*client);
    return;
  }
  std::string message = "PONG " + token + "\r\n";
  client->receiveMessage(message);
}
