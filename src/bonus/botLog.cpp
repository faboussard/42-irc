/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botLog.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 11:15:40 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/02 10:07:33 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "../../includes/Bot.hpp"
#include "../../includes/Log.hpp"

void Bot::logcreatSocketForApi(void) {
  std::ostringstream oss;
  oss << "fd" << _botSocketFd
      << ": Socket ready to communicate with IRC Server";
  Log::printLog(INFO_LOG, BOT_L, oss.str());
}

void Bot::logApiRequest(int fd, const std::string &apiHost) {
  std::ostringstream oss;
  oss << "fd" << fd << ": API request sent to " << apiHost;
  Log::printLog(INFO_LOG, BOT_L, oss.str());
}

void Bot::logApiResponse(int fd) {
  std::ostringstream oss;
  oss << "fd" << fd << ": Received API response";
  Log::printLog(INFO_LOG, BOT_L, oss.str());
}

void Bot::logApiTimeout(int fd, eBotCommand command) {
  std::string apiHost;
  switch (command) {
    case JOKE:
      apiHost = JOKEAPI_HOST;
      break;
    case INSULTME:
      apiHost = INSULTMEAPI_HOST;
      break;
    case ADVICE:
      apiHost = ADVICEAPI_HOST;
      break;
    case WEATHER:
      apiHost = WEATHERAPI_HOST;
      break;
    default:
      break;
  }
  std::ostringstream oss;
  oss << "fd" << fd << ": No response from " << apiHost << " (timeout)";
  Log::printLog(ERROR_LOG, BOT_L, oss.str());
}

#ifdef DEBUG
void Bot::debugLogServerMessageSplit(const std::string &clientNickname,
                                     const std::string &commandStr,
                                     const std::string &arg) {
  std::ostringstream oss;
  oss << "< SPLIT >: clientNickname: " CYAN << clientNickname
      << RESET " | Bot command: " CYAN << commandStr << " | Arg: " << arg
      << RESET;
  Log::printLog(DEBUG_LOG, PARSER, oss.str());
}

void Bot::debugLogParsedMessage(BotRequest request) {
  std::ostringstream oss;
  oss << "New request from " << request.clientNickname
      << " | Command: " << request.command << " "
      << commandToString(request.command);
  Log::printLog(DEBUG_LOG, PARSER, oss.str());
}

void Bot::debugLogWaitingRequests(void) {
  std::ostringstream oss;
  oss << _requestDatas.size() << " request(s) is/are waiting";
  Log::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif
