/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 07:52:23 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/25 16:06:07 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>

#include "../../includes/Bot.hpp"
#include "../../includes/Server.hpp"

#ifdef DEBUG
static void logBotCommand(const std::string &nick, Command command,
                          const std::string &arg);
#endif

/*============================================================================*/
/*       Bot setting                                                          */
/*============================================================================*/

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

/*============================================================================*/
/*       Bot command handling                                                 */
/*============================================================================*/

void Server::botCommands(Client *client, Command command,
                         const std::string &arg) {
#ifdef DEBUG
  logBotCommand(client->getNickname(), command, arg);
#endif
  (void)arg;
  if (command == BOT) {
    sendBotInstruction(*client);
    client->setBotLaunched(true);
  } else {
    if (client->botLaunched()) {
      sendRequestToBot(*client, command, arg);
    } else {
      sendNotice(*client, "Bot not launched yet. Please type /BOT to launch.");
    }
  }
}

void Server::sendBotInstruction(const Client &client) {
  const stringVector &instructions = _bot->getInstructions();
  stringVector::const_iterator itEnd = instructions.end();
  for (std::vector<std::string>::const_iterator it = instructions.begin();
       it != itEnd; ++it) {
    sendBotResponse(client, *it);
  }
}

void Server::sendRequestToBot(const Client &client, Command command,
                              const std::string &arg) {
  // Do PING to check the connection with bot ?
  //
  std::string request;
  {
    std::ostringstream oss;
    oss << client.getNickname() << " " << commandToString(command) << " "
        << arg;
    request = oss.str();
  }
  if (send(_bot->getIrcSocketFd(), request.c_str(), request.size(),
           MSG_NOSIGNAL) == -1)
    printLog(ERROR_LOG, SYSTEM, "Failed to send request to bot");
  else
    printLog(INFO_LOG, BOT_L, "IRC Server has sent a request: " + request);
}

void Server::sendBotResponse(const Client &client, const std::string &message) {
  std::ostringstream oss;
  oss << BOT_RESPONSE_HEADER << client.getNickname() << " :" << message
      << "\r\n";
  client.receiveMessage(oss.str());
}

#ifdef DEBUG
void logBotCommand(const std::string &nick, Command command,
                   const std::string &arg) {
  std::ostringstream oss;
  oss << "Server has received from " << nick << ": Command " << CYAN
      << Bot::commandToString(command) << RESET " | " << "arg " << CYAN << arg
      << RESET;
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif
