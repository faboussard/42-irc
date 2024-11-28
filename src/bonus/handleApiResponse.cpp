/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleApiResponse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:45 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/28 16:05:28 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <string>

#include "../../includes/Bot.hpp"
#include "../../includes/Log.hpp"

void Bot::handleApiResponse(int fd) {
  logApiResponse(fd);
  std::deque<BotRequest>::iterator it = _requestDatas.begin();
  std::deque<BotRequest>::iterator itEnd = _requestDatas.end();
  for (; it != itEnd; ++it) {
    std::cout << "command " << it->command << std::endl;
    if (it->fdForApi == fd) {
      std::cout << "FOUND" << std::endl;
      break;
    }
  }
  receiveResponseFromApi(it);
  sendResponseToServer(it);

  // remove from pollfds
  std::vector<struct pollfd>::iterator itPollEnd = _botPollFds.end();
  for (std::vector<struct pollfd>::iterator itPoll = _botPollFds.begin();
       itPoll != itPollEnd; ++itPoll) {
    if (itPoll->fd == fd) {
      _botPollFds.erase(itPoll);
      break;
    }
  }
  _requestDatas.erase(it);
  Log::printLog(INFO_LOG, BOT_L,
                "Request removed from queue. Remaining requests count: " +
                    toString(_requestDatas.size()));
}

void Bot::receiveResponseFromApi(std::deque<BotRequest>::iterator itRequest) {
  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), itRequest->fpForApi) != NULL) {
    itRequest->apiResponse += buffer;
  }
  pclose(itRequest->fpForApi);
  Log::printLog(INFO_LOG, BOT_L,
                "API response received: " + itRequest->apiResponse);
#ifdef DEBUG
  debugLogWaitingRequests();
#endif
}

void Bot::sendResponseToServer(std::deque<BotRequest>::iterator itRequest) {
  std::string content = parseResponse(itRequest->apiResponse);
  #ifdef DEBUG
    std::ostringstream oss;
    oss << "Response to send: " << content;
    Log::printLog(DEBUG_LOG, BOT_L, oss.str());
  #endif
  if (content.empty()) {
    Log::printLog(ERROR_LOG, BOT_L, "Response is empty");
    return;
  }
  std::string response =
      "PRIVMSG " + itRequest->clientNickname + " :" + content + "\r\n";
  sendMessageToServer(response);
  Log::printLog(INFO_LOG, BOT_L, "Response has to be sent to Server");
}

std::string Bot::parseResponse(const std::string &response) {
    std::string commandString = commandToString(_requestDatas.front().command);
    std::string parsedResponse;
    const std::string jokeCat =
        "     /\\_/\\\n"
        "    ( o.o )  Haha, what a joke!\n"
        "     > ^ <";

    const std::string adviceCat =
        "     /\\_/\\\n"
        "    ( o.o )  And be wise...\n"
        "     > ^ <";

    const std::string insultCat =
        "     /\\_/\\\n"
        "    ( o.o )  Ouch! That was harsh!\n"
        "     > ^ <";

    const std::string defaultCat =
        "     /\\_/\\\n"
        "    ( o.o )  I don't know what to say!\n"
        "     > ^ <";

    switch (_requestDatas.front().command) {
        case JOKE:
            parsedResponse = response + "\n" + jokeCat;
            break;
        // case WEATHER:
        //     parsedResponse = response; // À implémenter si nécessaire
        //     break;
        case ADVICE:
            parsedResponse = response + "\n" + adviceCat;
            break;
        case INSULTME:
            parsedResponse = response + "\n" + insultCat;
            break;
        default:
            parsedResponse = response + "\n" + defaultCat;
            break;
    }
#ifdef DEBUG
    std::ostringstream oss;
    oss << "Parsed response: " << parsedResponse;
    Server::printLog(DEBUG_LOG, BOT_L, oss.str());
#endif
    return (commandString + " : " + parsedResponse);
}
