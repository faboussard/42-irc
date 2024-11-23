/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleApiResponse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:45 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/23 23:12:33 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "../../includes/Bot.hpp"

void Bot::handleApiResponse(int fd) {
  Server::printLog(DEBUG_LOG, BOT_L, "Handling API response");

  std::string response = receiveResponseFromApi(fd);
  // sendResponseToServer(response);
}

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
    fd = -1;
    Server::printLog(INFO_LOG, BOT_L,
                     "API server closed the connection with Bot");
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
