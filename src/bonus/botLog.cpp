/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botLog.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 11:15:40 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/27 11:02:17 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"
#include "../../includes/Log.hpp"

void Bot::logcreatSocketForApi(void) {
  std::ostringstream oss;
  oss << "fd" << _botSocketFd
      << ": Socket ready to communicate with IRC Server at port " << _botPort;
  Log::printLog(INFO_LOG, BOT_L, oss.str());
}

void Bot::logApiResponse(int fd) {
  std::ostringstream oss;
  oss << "fd" << fd << ": Received API response";
  Log::printLog(INFO_LOG, BOT_L, oss.str());
}

void Bot::logApiConnectionClosed(int fd) {
  std::ostringstream oss;
  oss << "fd" << fd << ": API server closed the connection with Bot";
  Log::printLog(INFO_LOG, BOT_L, oss.str());
}

#ifdef DEBUG
void Bot::debugLogPipe(int ServerToBot0, int ServerToBot1, int BotToServer0,
                       int BotToServer1) {
  std::ostringstream oss;
  oss << "Pipes created: ServerToBot(" << ServerToBot0 << ", " << ServerToBot1
      << ") | BotToServer(" << BotToServer0 << ", " << BotToServer1 << ")";
  Log::printLog(DEBUG_LOG, BOT_L, oss.str());
}

void Bot::debugLogReadRequest(BotRequest request) {
  std::ostringstream oss;
  oss << "New request from " CYAN << request.clientNickname
      << RESET ": Command " << CYAN << request.command << RESET " | " << "arg "
      << CYAN << request.arg << RESET;
  Log::printLog(DEBUG_LOG, BOT_L, oss.str());
}

void Bot::debugLogWaitingRequests(void) {
  std::ostringstream oss;
  oss << _requestDatas.size() << " requests are waiting";
  Log::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif
