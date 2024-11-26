/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleApiResponse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:45 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/26 17:03:14 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <deque>
#include <string>

#include "../../includes/Bot.hpp"

#include "../../includes/Log.hpp"

void Bot::handleApiResponse(int fd) {
  (void)fd;
  // BotRequest *request;
  // std::deque<BotRequest>::iterator it = _requestDatas.begin();
  // std::deque<BotRequest>::iterator itEnd = _requestDatas.end();
  // for (; it != itEnd; ++it) {
  //   if (it->socketFd == fd) {
  //     request = &(*it);
  //     break;
  //   }
  // }
  // std::string response = receiveResponseFromApi(fd, it);

  // // sendResponseToServer(response);
  // (void)request;
}

// std::string Bot::receiveResponseFromApi(
//     int fd, std::deque<BotRequest>::iterator itRequest) {
//   char buffer[4096];
//   std::string response;
//   ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
//   if (bytesRead == -1) {
//     Server::printLog(ERROR_LOG, BOT_L,
//                      "Failed to receive response from API server: " +
//                          std::string(strerror(errno)));
//   } else if (bytesRead == 0) {
//     close(fd);
//     _requestDatas.erase(itRequest);
//     _server->removeApiSocketFdFromPoll(fd);
//     logApiConnectionClosed(fd);
// #ifdef DEBUG
//     debugLogWaitingRequests();
// #endif
//   } else {
//     buffer[bytesRead] = '\0';
//     logApiResponse(fd);
//     response = std::string(buffer);
//     Server::printLog(DEBUG_LOG, BOT_L, "Response: " + response);
//   }
//   return (response);
// }

void Bot::sendResponseToServer(const std::string &response) {
  (void)response;
  Log::printLog(INFO_LOG, BOT_L, "Response has to be sent to Server");
}
