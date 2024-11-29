/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleServerMessage.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:38 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/29 12:41:52 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "../../includes/Bot.hpp"
#include "../../includes/Log.hpp"
#include "../../includes/colors.hpp"
#include "../../includes/utils.hpp"

void Bot::handleServerMessage(void) {
  std::string requestBuffer = readMessageFromServer();
  Log::printLog(DEBUG_LOG, BOT_L, "Received from Server: " + requestBuffer);
  if (requestBuffer.empty()) return;
  // if (requestBuffer == PONG_MSG) {
  //   _connectedToServer = true;
  //   return;
  // }
  if (requestBuffer[0] != ':' ||
      requestBuffer.find("PRIVMSG") == std::string::npos)
    return;
  BotRequest newRequest = parseRequest(requestBuffer);
#ifdef DEBUG
  debugLogParsedMessage(newRequest);
#endif
  switch (newRequest.command) {
    case MENU:
      sendAsciiCatByCommand(&newRequest, MENU);
      break;
    case WEATHER:
      weather(&newRequest);
      break;
    case JOKE:
      joke(&newRequest);
      break;
    case INSULTME:
      insultMe(&newRequest);
      break;
    case ADVICE:
      advice(&newRequest);
      break;
    case UNKNOWN_BOT_COMMAND:
      unknownCommand(&newRequest);
      break;
    default:
      break;
  }
}

/*============================================================================*/
/*       Parse requests                                                       */
/*============================================================================*/

#include <cstdlib>
#include <ctime>

static eBotCommand selectCommand(const std::string& command) {
  std::srand(std::time(NULL));

  if (command == "MENU") {
    return (MENU);
  } else if (command == "JOKE") {
    return (JOKE);
  } else if (command == "INSULTME") {
    return (INSULTME);
  } else if (command == "ADVICE") {
    return (ADVICE);
  } else if (command == "WEATHER") {
    return (WEATHER);
  } else if (command == "RANDOM") {
    // Randomly select JOKE, ADVICE, or INSULTME
    int randomIndex = std::rand() % 3;
    switch (randomIndex) {
      case 0:
        return (JOKE);
      case 1:
        return (ADVICE);
      case 2:
        return (INSULTME);
    }
  }
  return (UNKNOWN_BOT_COMMAND);
}

BotRequest Bot::parseRequest(const std::string& requestBuffer) {
  Log::printLog(INFO_LOG, BOT_L, "Handling a new message from Server...");
  std::stringstream ss(requestBuffer);
  std::string clientNickname;
  std::string commandStr;
  // std::string arg;
  ss >> clientNickname >> commandStr >> commandStr >> commandStr;
  // std::getline(ss >> std::ws, arg);
#ifdef DEBUG
  debugLogServerMessageSplit(clientNickname, commandStr);
#endif
  commandStr = commandStr.substr(1);
  strToUpper(&commandStr);
  BotRequest newRequest(clientNickname.substr(1), selectCommand(commandStr));
  return (newRequest);
}
