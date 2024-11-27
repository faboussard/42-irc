/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleApiResponse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:45 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/27 11:23:39 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <deque>
#include <sstream>
#include <string>

#include "../../includes/Bot.hpp"

void Bot::handleApiResponse(int fd) {
   std::string userClientNickname;
  std::deque<BotRequest>::iterator it = _requestDatas.begin();
  std::deque<BotRequest>::iterator itEnd = _requestDatas.end();
  for (; it != itEnd; ++it) {
    if (it->socketFd == fd) {
      userClientNickname = it->clientNickname;
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
std::string parsedResponse = parseResponse(response);
  sendResponseToServer(userClientNickname, parsedResponse);
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
    Server::printLog(DEBUG_LOG, BOT_L, "ReceiveResponseFromApi - Response: " + response);
  }
  return (response);
}

void Bot::sendResponseToServer(const std::string &clientNickname, const std::string &response) {
    _server->addBotResponseToQueue(clientNickname, response);
    char notify = 1;
    if (write(_pipeBotToServer[1], &notify, 1) == -1)
        _server->printLog(INFO_LOG, BOT_L, "Failed to write to pipe in sendResponseToServer");
}

std::string Bot::parseResponse(const std::string &response) {
  std::string parsedResponse;
 switch (_requestDatas.front().command) {
    case NUMBERS:
      parsedResponse = parseNumbersResponse(response);
      break;
    case JOKE:
      parsedResponse = response;
      break;
    default:
      parsedResponse = response;
      break;
  }
  #ifdef DEBUG
  std::ostringstream oss;
  oss << "Parsed response: " << parsedResponse;
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
  #endif
  return (parsedResponse);
}

std::string Bot::parseNumbersResponse(const std::string &response) {
  std::string parsedResponse;
  std::string line;
  std::string lastLine;
  std::istringstream iss(response);
  while (std::getline(iss, line))
    lastLine = line;
  parsedResponse = lastLine;
  return (parsedResponse);
}