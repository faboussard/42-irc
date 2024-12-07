/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 15:09:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/05 21:52:20 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "../../includes/Server.hpp"

#include "../../includes/Log.hpp"

void Server::ping(const Client &client, const std::string &token) {
  if (token.empty()) {
    send409NoOrigin(client);
    return;
  }
  {
    std::ostringstream oss;
    oss << client.getNickname() << "sent PING " << token;
    Log::printLog(INFO_LOG, CLIENT, oss.str());
    std::string message = "PONG " + token + "\r\n";
    client.receiveMessage(message);
  }
  {
    std::ostringstream oss;
    oss << "PONG " << token << " sent to " << client.getNickname();
    Log::printLog(INFO_LOG, REPLY, oss.str());
  }
}
