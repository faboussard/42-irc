/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 07:52:23 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/23 23:07:41 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <poll.h>

#include <algorithm>
#include <string>
#include <vector>

#include "../../includes/Bot.hpp"
#include "../../includes/Server.hpp"

void logRequestSentToBot(const std::string &nick, Command command,
                         const std::string &arg);
#ifdef DEBUG
static void logBotCommand(const std::string &nick, Command command,
                          const std::string &arg);
#endif

/*============================================================================*/
/*      Poll handling                                                         */
/*============================================================================*/

void Server::addBotToPoll(int pipeFdServerToBot, int pipeFdBotToServer) {
  struct pollfd botPollFromServer;
  botPollFromServer.fd = pipeFdServerToBot;
  botPollFromServer.events = POLLIN;
  botPollFromServer.revents = 0;
  _pollFds.push_back(botPollFromServer);

  struct pollfd botPollToServer;
  botPollToServer.fd = pipeFdBotToServer;
  botPollToServer.events = POLLIN;
  botPollToServer.revents = 0;
  _pollFds.push_back(botPollToServer);
}

void Server::addBotSocketFdToPoll(int newFd) {
  _botToApiSocketFds.push_back(newFd);
  struct pollfd botPollApi;
  botPollApi.fd = newFd;
  botPollApi.events = POLLIN;
  botPollApi.revents = 0;
  _pollFds.push_back(botPollApi);
}

void Server::removeApiSocketFdFromPoll(int soketFd) {
  for (size_t i = 0; i < _pollFds.size(); ++i) {
    if (_pollFds[i].fd == soketFd) {
      _pollFds.erase(_pollFds.begin() + i);
      break;
    }
  }
  std::vector<int>::iterator it = std::find(_botToApiSocketFds.begin(),
                                            _botToApiSocketFds.end(), soketFd);
  if (it != _botToApiSocketFds.end())
    _botToApiSocketFds.erase(it);
}

const std::vector<int> &Server::getBotToApiSocketFds(void) const {
  return (_botToApiSocketFds);
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
  std::string clientNickname = client.getNickname();
  const stringVector &instructions = _bot->getInstructions();
  stringVector::const_iterator itEnd = instructions.end();
  for (std::vector<std::string>::const_iterator it = instructions.begin();
       it != itEnd; ++it) {
    std::ostringstream oss;
    oss << BOT_RESPONSE_HEADER << clientNickname << " :" << *it << "\r\n";
    client.receiveMessage(oss.str());
  }
}

void Server::sendRequestToBot(const Client &client, Command command,
                              const std::string &arg) {
  const std::string &nick = client.getNickname();
  BotRequest newRequest(nick, command, arg);
  _bot->receiveRequestInQueue(newRequest);

  logRequestSentToBot(nick, command, arg);
}

void Server::addBotResponseToQueue(const std::string &response) {
  _responsesFromBot.push(response);
}

void Server::handleBotResponse(int serverFdListenBot) {
  // Clear the notification in pipe
  char buffer[2] = {0};
  read(serverFdListenBot, &buffer, 1);

  if (_responsesFromBot.empty())
    return;
  std::stringstream ss(_responsesFromBot.front());
  _responsesFromBot.pop();

  std::string clientNickname;
  std::string response;
  ss >> clientNickname;
  std::getline(ss >> std::ws, response);
    // Response must not has \n for PRIVMSG from bot

  std::ostringstream oss;
  oss << BOT_RESPONSE_HEADER << clientNickname << " :" << response << "\r\n";
  findClientByNickname(clientNickname)->receiveMessage(oss.str());
}

/*============================================================================*/
/*       Logs                                                                 */
/*============================================================================*/

void logRequestSentToBot(const std::string &nick, Command command,
                         const std::string &arg) {
  std::ostringstream oss;
  oss << "IRC Server has sent a request from " << nick
      << Bot::botCommandStr(command) << " " << arg;
  Server::printLog(INFO_LOG, BOT_L, oss.str());
}

#ifdef DEBUG
void logBotCommand(const std::string &nick, Command command,
                   const std::string &arg) {
  std::ostringstream oss;
  oss << "Server has received from " << nick << ": Command " << CYAN
      << Bot::botCommandStr(command) << RESET " | " << "arg " << CYAN << arg
      << RESET;
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif
