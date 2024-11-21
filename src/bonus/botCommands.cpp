/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 07:52:23 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/21 11:09:59 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>

#include "../../includes/Bot.hpp"
#include "../../includes/Server.hpp"

void Server::addBot(struct pollfd *pollFdIrc, struct pollfd *pollFdApi) {
  _pollFds.push_back(*pollFdIrc);
  _pollFds.push_back(*pollFdApi);

  struct sockaddr_in botAddr;
  socklen_t len = sizeof(botAddr);

  int botFd = accept(_socketFd, reinterpret_cast<sockaddr *>(&botAddr), &len);
  if (botFd == -1) {
    printLog(ERROR_LOG, SYSTEM, "Failed to add bot");
    return;
  }
  if (fcntl(botFd, F_SETFL, O_NONBLOCK) == -1) {
    printLog(ERROR_LOG, SYSTEM, "fcntl() failed");
    return;
  }
  _bot->setBotFdInServer(botFd);

  struct pollfd newPoll;
  newPoll.fd = botFd;
  newPoll.events = POLLIN;
  newPoll.revents = 0;
  _pollFds.push_back(newPoll);

  std::ostringstream oss;
  oss << "Bot has joined to " << gConfig->getParam(NETWORK) << " at fd"
      << botFd;
  printLog(INFO_LOG, SYSTEM, oss.str());
}

void Server::botCommands(Client *client, Command command,
                         const std::string &arg) {
  (void)arg;
  if (command == BOT) {
    sendBotInstruction(*client);
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

void Server::sendBotInstruction(const Client &client) {
  const std::vector<std::string> &instructions = _bot->getInstructions();
  std::vector<std::string>::const_iterator itEnd = instructions.end();
  for (std::vector<std::string>::const_iterator it = instructions.begin();
       it != itEnd; ++it) {
    sendBotResponse(client, *it);
  }
}

void Server::sendBotResponse(const Client &client, const std::string &message) {
  std::ostringstream oss;
  oss << BOT_RESPONSE_HEADER << client.getNickname() << " :" 
      << message << "\r\n";
  client.receiveMessage(oss.str());
}
