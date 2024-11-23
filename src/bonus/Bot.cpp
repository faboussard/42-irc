/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:01:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/23 18:07:22 by yusengok         ###   ########.fr       */
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
  std::vector<int>::iterator itEnd = _socketFds.end();
  for (std::vector<int>::iterator it = _socketFds.begin(); it != itEnd; ++it) {
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
  _server->set_botToApiSocketFds(&_socketFds);
  Server::printLog(INFO_LOG, BOT_L, "Ready to communicate with IRC server");

  // TEST
  // std::string apiHost = "v2.jokeapi.dev";
  // std::string apiHost = "numbersapi.com";
  // int apiPort = 80;
  // createSocketForApi(0, apiHost, apiPort);
  // connectToApiServer(0, apiHost, apiPort);
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

#ifdef DEBUG
void debugLogPipe(int ServerToBot0, int ServerToBot1, int BotToServer0,
                  int BotToServer1) {
  std::ostringstream oss;
  oss << "Pipes created: ServerToBot(" << ServerToBot0 << ", " << ServerToBot1
      << ") | BotToServer(" << BotToServer0 << ", " << BotToServer1 << ")";
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif
