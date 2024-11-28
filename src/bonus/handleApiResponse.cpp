/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleApiResponse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:45 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/27 21:29:28 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
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
  Log::printLog(INFO_LOG, BOT_L, "Request removed from queue. Remaining requests count: " +
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
  // std::string content = parseResponse(itRequest);
  // if (content.empty()) return;

  /* test */
  std::string content = "Yeah, but you're ugly and confused.";

  std::string response =
      "PRIVMSG " + itRequest->clientNickname + " :" + content + "\r\n";
  sendMessageToServer(response);
  Log::printLog(INFO_LOG, BOT_L, "Response has to be sent to Server");
}
