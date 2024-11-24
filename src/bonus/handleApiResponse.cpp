/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleApiResponse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:45 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/24 23:18:40 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "../../includes/Bot.hpp"

void Bot::handleApiResponse(int fd) {
  Server::printLog(DEBUG_LOG, BOT_L, "Handling API response");

  std::string response = receiveResponseFromApi(fd);
  
  // BotRequest *request;
  // std::deque<BotRequest>::iterator it = _requestDatas.begin();
  // std::deque<BotRequest>::iterator itEnd = _requestDatas.end();
  // for (; it != itEnd; ++it) {
  //   if (it->socketFd == fd) {
  //     request = &(*it);
  //     break;
  //   }
  // }
  // // sendResponseToServer(response);
  // _requestDatas.erase(it);
  // (void)request;
}

// [2024-11-24 23:07:06] DEBUG   [Bot] Request sent: GET /42 HTTP/1.1
// Host: numbersapi.com
// Connection: close

// [2024-11-24 23:07:06] INFO    [Bot] Received a response from API server
// [2024-11-24 23:07:06] DEBUG   [Bot] Handling API response
// [2024-11-24 23:07:06] INFO    [Bot] fd9: Received API response
// [2024-11-24 23:07:06] DEBUG   [Bot] Response: HTTP/1.1 200 OK
// Server: nginx/1.4.6 (Ubuntu)
// Date: Sun, 24 Nov 2024 21:52:21 GMT
// Content-Type: text/plain; charset=utf-8
// Content-Length: 109
// Connection: close
// X-Powered-By: Express
// Access-Control-Allow-Origin: *
// X-Numbers-API-Number: 42
// X-Numbers-API-Type: trivia
// Pragma: no-cache
// Cache-Control: no-cache
// Expires: 0
// ETag: W/"6d-gWsWRKWkaWeOc6vuJxXsGbg0KuI"
// Last-Modified: 1732485141
// Cache-Control: no-store, no-cache, must-revalidate, proxy-revalidate, max-age=0

// 42 is the number of museums in Amsterdam (Netherlands has the highest concentration of museums in the world).
// [2024-11-24 23:07:06] INFO    [Bot] Received a response from API server
// [2024-11-24 23:07:06] DEBUG   [Bot] Handling API response
// [2024-11-24 23:07:06] INFO    [Bot] fd9: API server closed the connection with Bot
// ircserv(5765,0x7ff85ba98dc0) malloc: *** error for object 0x7369206769666e6f: pointer being freed was not allocated

std::string Bot::receiveResponseFromApi(int fd) {
  char buffer[4096];
  std::string response;
  ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
  if (bytesRead == -1) {
    Server::printLog(ERROR_LOG, BOT_L,
                     "Failed to receive response from API server: "
                    + std::string(strerror(errno)));
  } else if (bytesRead == 0) {
    close(fd);
    _server->removeApiSocketFdFromPoll(fd);
    std::ostringstream oss;
    oss << "fd" << fd << ": API server closed the connection with Bot";
    Server::printLog(INFO_LOG, BOT_L, oss.str());
    fd = -1;
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
  write(_pipeBotToServer[1], &notify, 1);
  Server::printLog(INFO_LOG, BOT_L, "Response has to be sent to Server");
}
