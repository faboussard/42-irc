/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 07:52:23 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/20 13:25:28 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

#include "../../includes/Bot.hpp"

void Server::addBot(Bot *bot) {
  _bot = bot;

  struct sockaddr_in botadd;
  socklen_t len = sizeof(botadd);

  int botFd = accept(_socketFd, reinterpret_cast<sockaddr *>(&botadd), &len);
  if (botFd == -1) {
    printLog(ERROR_LOG, SYSTEM, "Failed to add bot");
    return;
  }
  if (fcntl(botFd, F_SETFL, O_NONBLOCK) == -1) {
    printLog(ERROR_LOG, SYSTEM, "fcntl() failed");
    return;
  }

  struct pollfd newPoll;
  newPoll.fd = botFd;
  newPoll.events = POLLIN;
  newPoll.revents = 0;
  _pollFds.push_back(newPoll);

  // std::string botIp = inet_ntoa(botadd.sin_addr);
  // struct hostent *host =
  //     gethostbyaddr(&botadd.sin_addr, sizeof(botadd.sin_addr), AF_INET);
  // std::string hostName;
  // if (host->h_name == NULL || sizeof(host->h_name) == 0 ||
  //     static_cast<size_t>(host->h_length) > gConfig->getLimit(HOSTLEN))
  //   hostName = botIp;
  // else
  //   hostName = host->h_name;
}

void Server::botCommands(Client *client, Command command,
                         const std::string &arg) {
  (void)arg;
  if (command == BOT) {
    // sendBotCommandsList(client);
    client->setBotLaunched(true);
  } else {
#ifdef DEBUG
    std::ostringstream oss;
    oss << "Bot command received from " << client->getNickname();
    printLog(DEBUG_LOG, BOT_L, oss.str());
#endif
    if (client->botLaunched()) {
      // sendToBot(client, command, arg);
    } else {
      // sendBotNotLaunched(client);
    }
  }
}
