/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleServerMessage.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:38 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/27 14:02:32 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <string>

#include "../../includes/Bot.hpp"
#include "../../includes/Log.hpp"
#include "../../includes/Parser.hpp"

void Bot::handleServerMessage(void) {
    std::string requestBuffer = readMessageFromServer();
    if (requestBuffer.empty()) return;
  BotRequest newRequest = parseRequest(requestBuffer);
  switch (newRequest.command) {
    case PONG:
      _connectedToServer = true;
      break;
    case BOT:
      bot(&newRequest);
      break;
    case NUMBERS:
      numbers(&newRequest);
      break;
    case JOKE:
      joke(&newRequest);
      break;
    case FILM:
      film(&newRequest);
      break;
    case INSULTME:
      insultMe(&newRequest);
      break;
    default:
      break;
  }
}

/*============================================================================*/
/*       Parse requests                                                       */
/*============================================================================*/

BotRequest Bot::parseRequest(const std::string &requestBuffer) {
  Log::printLog(INFO_LOG, BOT_L, "Handling a new message from Server...");
#ifdef DEBUG
  Log::printLog(DEBUG_LOG, BOT_L, "Message received: " + requestBuffer);
#endif
  std::stringstream ss(requestBuffer);
  std::string commandStr;
  std::string clientNickname;
  std::string arg;
  ss >> commandStr >> clientNickname;
  std::getline(ss >> std::ws, arg);
  Log::printLog(DEBUG_LOG, BOT_L,
                "< PARSED> Command: " + commandStr +
                " | Nickname: " + clientNickname + " | Argument: " + arg);
  BotRequest newRequest(clientNickname, Parser::choseCommand(commandStr), arg);
  return (newRequest);
}

/*============================================================================*/
/*       Send to API                                                          */
/*============================================================================*/

void Bot::sendRequestToApi(const std::string &request, int socketFd) {
  ssize_t bytesSent = send(socketFd, request.c_str(), request.size(), 0);
  if (bytesSent == -1) {
    Log::printLog(ERROR_LOG, BOT_L,
                  "Failed to send request to API server: " +
                      std::string(strerror(errno)));
  } else {
    Log::printLog(INFO_LOG, BOT_L, "Sent request to API server");
    Log::printLog(DEBUG_LOG, BOT_L, "Request sent: " + request);
  }
}
