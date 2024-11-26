/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:01:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/26 08:50:50 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

#include <unistd.h>

#include <cerrno>
#include <string>
#include <vector>

#include "../../includes/Server.hpp"

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Bot::Bot(Server *server)
    : _name(BOT_NAME),
      _server(server) {
  _instructions.push_back(BOT1);
  _instructions.push_back(BOT2);
  _instructions.push_back(BOT3);
  _instructions.push_back(BOT4);
  _instructions.push_back(BOT5);
  _instructions.push_back(BOT6);
  _instructions.push_back(BOT7);
  _instructions.push_back(BOT8);
  _instructions.push_back(BOT9);
}

Bot::~Bot(void) {
  close(_pipeServerToBot[0]);
  close(_pipeServerToBot[1]);
  close(_pipeBotToServer[0]);
  close(_pipeBotToServer[1]);
  const std::vector<int>socketFds = _server->getBotToApiSocketFds();
  std::vector<int>::const_iterator itEnd = socketFds.end();
  for (std::vector<int>::const_iterator it = socketFds.begin(); it != itEnd;
      ++it) {
    close(*it);
  }
}

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

int Bot::getServerToBotPipe0(void) const { return _pipeServerToBot[0]; }

int Bot::getBotToServerPipe0(void) const { return _pipeBotToServer[0]; }

const std::vector<std::string> &Bot::getInstructions(void) const {
  return (_instructions);
}

/*============================================================================*/
/*       Setters                                                              */
/*============================================================================*/

/*============================================================================*/
/*       Bot launch                                                           */
/*============================================================================*/

void Bot::runBot(void) {
  if (pipe(_pipeServerToBot) == -1 || pipe(_pipeBotToServer) == -1)
    Server::printLog(ERROR_LOG, BOT_L, "Failed to create pipe");
#ifdef DEBUG
  debugLogPipe(_pipeServerToBot[0], _pipeServerToBot[1], _pipeBotToServer[0],
               _pipeBotToServer[1]);
#endif
  Server::printLog(INFO_LOG, BOT_L, "Ready to communicate with IRC server");
}

/*============================================================================*/
/*       Helper functions & log                                               */
/*============================================================================*/

std::string Bot::botCommandStr(Command command) {
  std::string commandStr;
  switch (command) {
    case BOT:
      return ("BOT");
    case WEATHER:
      return ("WEATHER");
    case TRANSLATE:
      return ("TRANSLATE");
    default:
      return ("");
  }
}

/*============================================================================*/
/*       Logs                                                                 */
/*============================================================================*/

void Bot::logcreatSocketForApi(const BotRequest &request) {
  std::ostringstream oss;
  oss << "fd" << request.socketFd << ": Ready to communicate with API Server "
      << request.apiHost << ":" << request.apiPort;
  Server::printLog(INFO_LOG, BOT_L, oss.str());
}

void Bot::logApiResponse(int fd) {
  std::ostringstream oss;
  oss << "fd" << fd << ": Received API response";
  Server::printLog(INFO_LOG, BOT_L, oss.str());
}

void Bot::logApiConnectionClosed(int fd) {
  std::ostringstream oss;
  oss << "fd" << fd << ": API server closed the connection with Bot";
  Server::printLog(INFO_LOG, BOT_L, oss.str());
}

#ifdef DEBUG
void Bot::debugLogPipe(int ServerToBot0, int ServerToBot1, int BotToServer0,
                  int BotToServer1) {
  std::ostringstream oss;
  oss << "Pipes created: ServerToBot(" << ServerToBot0 << ", " << ServerToBot1
      << ") | BotToServer(" << BotToServer0 << ", " << BotToServer1 << ")";
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}

void Bot::debugLogReadRequest(BotRequest request) {
  std::ostringstream oss;
  oss << "New request from " CYAN << request.clientNickname
      << RESET ": Command " << CYAN << request.command
      << RESET " | " << "arg " << CYAN << request.arg << RESET;
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}

void Bot::debugLogWaitingRequests(void) {
    std::ostringstream oss;
    oss << _requestDatas.size() <<  " requests are waiting";
    Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif
