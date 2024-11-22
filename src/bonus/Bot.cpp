/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:01:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/22 21:43:12 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

#include <unistd.h>

#include <cerrno>
#include <string>
#include <vector>

#include "../../includes/Server.hpp"

#ifdef DEBUG
static void debugLogPipe(int ServerToBot0, int ServerToBot1, int BotToServer0,
                         int BotToServer1);
static void debugLogcreatSocketForApi(int port, int fd);
static void debugLogReadRequest(BotRequest request);
#endif

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Bot::Bot(Server *server)
    : _name(BOT_NAME),
      _server(server),
      _apiPort(API_PORT),
      _apiSocketFd(-1) {
  if (pipe(_pipeServerToBot) == -1 || pipe(_pipeBotToServer) == -1)
    Server::printLog(ERROR_LOG, BOT_L, "Failed to create pipe");
  _instructions.push_back(BOT1);
  _instructions.push_back(BOT2);
  _instructions.push_back(BOT3);
  _instructions.push_back(BOT4);
  _instructions.push_back(BOT5);
  _instructions.push_back(BOT6);
  _instructions.push_back(BOT7);
  _instructions.push_back(BOT8);
  _instructions.push_back(BOT9);
#ifdef DEBUG
  debugLogPipe(_pipeServerToBot[0], _pipeServerToBot[1], _pipeBotToServer[0],
               _pipeBotToServer[1]);
#endif
}

Bot::~Bot(void) {
  close(_pipeServerToBot[0]);
  close(_pipeServerToBot[1]);
  close(_pipeBotToServer[0]);
  close(_pipeBotToServer[1]);
  close(_apiSocketFd);
}

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

int Bot::getServerToBotPipe0(void) const { return _pipeServerToBot[0]; }

int Bot::getBotToServerPipe0(void) const { return _pipeBotToServer[0]; }

int Bot::getApiSocketFd(void) const { return _apiSocketFd; }

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
  Server::printLog(INFO_LOG, BOT_L, "Ready to communicate with IRC server");

  if (_server->getPort() == API_PORT) _apiPort = API_PORT2;
  createSocketForApi();
  listenApiServer();
}

void Bot::createSocketForApi(void) {
  _apiSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_apiSocketFd == -1) {
    throw std::runtime_error("Failed to create socket");
  }
  int en = 1;
  if (setsockopt(_apiSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) ==
      -1) {
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket");
  }
  if (fcntl(_apiSocketFd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket");
  }
#ifdef DEBUG
  debugLogcreatSocketForApi(_apiPort, _apiSocketFd);
#endif
}

void Bot::listenApiServer(void) {
  _apiAddress.sin_family = AF_INET;
  _apiAddress.sin_addr.s_addr = INADDR_ANY;
  _apiAddress.sin_port = htons(_apiPort);
  if (bind(_apiSocketFd, (struct sockaddr *)&_apiAddress,
           sizeof(_apiAddress)) == -1) {
    throw std::runtime_error(
        "Failed to bind socket. Port might be used elsewhere");
  }
  if (listen(_apiSocketFd, SOMAXCONN) == -1) {
    throw std::runtime_error("Failed to listen on socket");
  }
}

/*============================================================================*/
/*       Client requests handling                                             */
/*============================================================================*/

void Bot::receiveRequestInQueue(const std::string &request) {
  _requests.push(request);
  char notify = 1;
  write(_pipeServerToBot[1], &notify, sizeof(notify));
}

void Bot::handleRequest(void) {
  // Clear the notification in pipe
  char buffer[2] = {0};
  read(_pipeServerToBot[0], buffer, sizeof(buffer) - 1);

  BotRequest request = readRequest();

  // std::string httpRequest = parseRequest(request);
  // if (httpRequest.empty())
  // send error message to server that sends it to client
  // return;

  // sendRequestToApi(httpRequest);
}

BotRequest Bot::readRequest(void) {
  if (!_requests.empty()) {
    std::stringstream ss(_requests.front());
    _requests.pop();
    std::string clientNickname;
    std::string command;
    std::string arg;
    ss >> clientNickname;
    ss >> command;
    std::getline(ss >> std::ws, arg);
    BotRequest request(clientNickname, command, arg);
#ifdef DEBUG
    debugLogReadRequest(request);
#endif
    return (request);
  }
  return (BotRequest("", "", ""));
}

// std::string Bot::parseRequest(const BotRequest &request) {
//   (void)request;
//   // if (request.command == "WEATHER")
//   // parse for weather
//   // else if (request.command == "TRANSLATE")
//   // parse for translate
// }

// void Bot::sendRequestToApi(const std::string &request) {
//   // send request to API server
// }

/*============================================================================*/
/*       API Servers responses handling                                       */
/*============================================================================*/

void Bot::handleApiResponse(void) {
  Server::printLog(DEBUG_LOG, BOT_L, "Handling API response");
  std::string response;
  sendResponseToServer(response);
}

void Bot::sendResponseToServer(const std::string &response) {
  _server->addBotResponseToQueue(response);
  char notify = 1;
  write(_pipeBotToServer[1], &notify, 1);
  Server::printLog(INFO_LOG, BOT_L, "Response has to be sent to Server");
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

void debugLogcreatSocketForApi(int port, int fd) {
  std::ostringstream oss;
  oss << "Socket for API created at Port " << port << " (fd" << fd << ")";
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}

void debugLogReadRequest(BotRequest request) {
  std::ostringstream oss;
  oss << "New request from " CYAN << request.clientNickname
      << RESET ": Command " << CYAN << request.command
      << RESET " | " << "arg " << CYAN << request.arg << RESET;
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif
