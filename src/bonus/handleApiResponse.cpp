/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleApiResponse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:45 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/26 12:16:36 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <deque>
#include <string>
#include <sstream>

#include "../../includes/Bot.hpp"

void Bot::handleApiResponse(int fd) {
  std::deque<BotRequest>::iterator it = _requestDatas.begin();
  std::deque<BotRequest>::iterator itEnd = _requestDatas.end();
  for (; it != itEnd; ++it) {
    if (it->socketFd == fd) {
      break;
    }
  }
  if (it == itEnd) {
    std::ostringstream oss;
    oss << "Request not found for fd: " << fd;
    Server::printLog(ERROR_LOG, BOT_L, oss.str());
    return;
  }
  std::string response = receiveResponseFromApi(fd, it);
  #ifdef DEBUG
  std::ostringstream oss;
  oss << "Response: " << response;
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
  #endif
  sendResponseToServer(response);
}


std::string Bot::receiveResponseFromApi(
    int fd, std::deque<BotRequest>::iterator request) {
  char buffer[4096];
  std::string response;
  ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
  if (bytesRead == -1) {
    Server::printLog(ERROR_LOG, BOT_L,
                     "Failed to receive response from API server: " +
                         std::string(strerror(errno)));
  } else if (bytesRead == 0) {
    close(fd);
    _requestDatas.erase(request);
    _server->removeApiSocketFdFromPoll(fd);
    logApiConnectionClosed(fd);
#ifdef DEBUG
    debugLogWaitingRequests();
#endif
  } else {
    buffer[bytesRead] = '\0';
    logApiResponse(fd);
    response = std::string(buffer);
    Server::printLog(DEBUG_LOG, BOT_L, "Response: " + response);
  }
  return (response);
}

void Bot::sendResponseToServer(const std::string &response) {
  _server->addBotResponseToQueue(response);
  char notify = 1;
  if (write(_pipeBotToServer[1], &notify, 1) == -1)
  Server::printLog(INFO_LOG, BOT_L, "Response has to be sent to Server");
  return;
}
