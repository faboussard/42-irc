/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 07:52:23 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/22 21:47:31 by yusengok         ###   ########.fr       */
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

void Server::addBotToPoll(int pipeFdServerToBot, int pipeFdBotToServer,
                          int botFdListenApi) {
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

  struct pollfd botPollApi;
  botPollApi.fd = botFdListenApi;
  botPollApi.events = POLLIN;
  botPollApi.revents = 0;
  _pollFds.push_back(botPollApi);
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
  std::ostringstream oss;
  oss << client.getNickname() << " " << Bot::botCommandStr(command) << " "
      << arg;
  std::string request = oss.str();
  _bot->receiveRequestInQueue(request);
  printLog(INFO_LOG, BOT_L, "IRC Server has sent a request: " + request);
}

void Server::addBotResponseToQueue(const std::string &response) {
  _responsesFromBot.push(response);
}

void Server::handleBotResponse(int serverFdListenBot) {
  // Clear the notification in pipe
  char buffer[2] {0};
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
