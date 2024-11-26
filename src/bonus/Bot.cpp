/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:01:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/25 16:03:45 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

#include <cerrno>
#include <string>
#include <vector>

#include "../../includes/Server.hpp"

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Bot::Bot(Server *server)
    : _server(server),
      _name(BOT_NAME),
      _ircPort(IRC_PORT),
      _ircSocketFd(-1),
      _apiPort(API_PORT),
      _apiSocketFd(-1) {
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

Bot::~Bot(void) { closeBot(); }

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

int Bot::getIrcSocketFd(void) const { return _ircSocketFd; }

int Bot::getApiSocketFd(void) const { return _apiSocketFd; }

const std::vector<std::string> &Bot::getInstructions(void) const {
  return (_instructions);
}

/*============================================================================*/
/*       Setters                                                              */
/*============================================================================*/

void Bot::setBotFdInServer(int fd) { _botFdInServer = fd; }

/*============================================================================*/
/*       Bot launch                                                           */
/*============================================================================*/

void Bot::runBot(void) {
  if (_server->getPort() == IRC_PORT) _ircPort = IRC_PORT2;
  if (_server->getPort() == API_PORT) _apiPort = API_PORT2;
  createSockets();
  connectToIrcServer();
  listenApiServer();
  _pollFdIrc.fd = _ircSocketFd;
  _pollFdIrc.events = POLLIN;
  _pollFdIrc.revents = 0;
  _pollFdApi.fd = _apiSocketFd;
  _pollFdApi.events = POLLIN;
  _pollFdApi.revents = 0;
  _server->addBot(&_pollFdIrc, &_pollFdApi);
}

void Bot::createSockets(void) {
  // Creat socket for IRC Server
  _ircSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_ircSocketFd == -1) throw std::runtime_error("Failed to create socket");

  int en = 1;
  if (setsockopt(_ircSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket");

  if (fcntl(_ircSocketFd, F_SETFL, O_NONBLOCK) == -1)
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket");
    // int flags = fcntl(_ircSocketFd, F_GETFL, 0);
    // fcntl(_ircSocketFd, F_SETFL, flags | O_NONBLOCK);
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "Socket for IRC server created at Port " << _ircPort << " (fd"
        << _ircSocketFd << ")";
    Server::printLog(DEBUG_LOG, BOT_L, oss.str());
  }
#endif

  // Creat socket for API Server
  _apiSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_apiSocketFd == -1) {
    throw std::runtime_error("Failed to create socket");
  }
  en = 1;
  if (setsockopt(_apiSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) ==
      -1) {
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket");
  }
  if (fcntl(_apiSocketFd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket");
  }
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "Socket for API server created at Port " << _apiPort << " (fd"
        << _apiSocketFd << ")";
    Server::printLog(DEBUG_LOG, BOT_L, oss.str());
  }
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

void Bot::connectToIrcServer(void) {
  _ircAddress.sin_family = AF_INET;
  _ircAddress.sin_port = htons(_server->getPort());
  _ircAddress.sin_addr.s_addr = inet_addr(LOCALHOST);
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "Attempting to connect to IRC server at " LOCALHOST << ":"
        << _server->getPort();
    Server::printLog(DEBUG_LOG, BOT_L, oss.str());
  }
#endif

  if (connect(_ircSocketFd, (struct sockaddr *)&_ircAddress,
              sizeof(_ircAddress)) == -1) {
    if (errno != EINPROGRESS) {
      throw std::runtime_error(BOT_CONNECTION_FAILED_IRC);
    }

    // Wait for connection to be established
    struct pollfd pollFd;
    pollFd.fd = _ircSocketFd;
    pollFd.events = POLLOUT;

    int pollRet = poll(&pollFd, 1, -1);
    if (pollRet == -1) {
      throw std::runtime_error(BOT_CONNECTION_FAILED_IRC);
    } else if (pollRet == 0) {
      throw std::runtime_error("Failed to connect to IRC server: Poll timeout");
    }

    // Check if connection was successful
    int socketError;
    socklen_t len = sizeof(socketError);
    getsockopt(_ircSocketFd, SOL_SOCKET, SO_ERROR, &socketError, &len);
    if (socketError != 0) {
      throw std::runtime_error(BOT_CONNECTION_FAILED_IRC);
    }
  }
  Server::printLog(INFO_LOG, BOT_L, "Bot is ready to listen to IRC server");
}

/*============================================================================*/
/*       Bot shut down                                                        */
/*============================================================================*/

void Bot::closeBot(void) {
  if (_botFdInServer != -1) {
    close(_botFdInServer);
    _botFdInServer = -1;
    Server::printLog(INFO_LOG, SYSTEM, "Bot disconnected");
    if (_ircSocketFd != -1) {
      close(_ircSocketFd);
      _ircSocketFd = -1;
      Server::printLog(DEBUG_LOG, BOT_L, "IRC socket closed");
    }
    if (_apiSocketFd != -1) {
      close(_apiSocketFd);
      _apiSocketFd = -1;
      Server::printLog(DEBUG_LOG, BOT_L, "API socket closed");
    }
  }
}

/*============================================================================*/
/*       Client requests handling                                             */
/*============================================================================*/

void Bot::handleRequest(void) {
  // Block other requests until the current request is handled

  char buffer[1024] = {0};
  std::memset(buffer, 0, sizeof(buffer));
  int valread = recv(_ircSocketFd, buffer, sizeof(buffer), 0);

  if (valread == -1) {
    std::ostringstream oss;
    oss << "Error occurred while receiving a message from IRC Server. :"
        << strerror(errno);
    Server::printLog(ERROR_LOG, BOT_L, oss.str());
    return;
  }
  if (valread == 0) {
    std::ostringstream oss;
    oss << "Connection closed by IRC Server";
    Server::printLog(ERROR_LOG, BOT_L, oss.str());
    closeBot();
    return;
  }
  Server::printLog(INFO_LOG, BOT_L, "Handling request: " + std::string(buffer));

  std::stringstream ss;
  ss << buffer;
  std::string clientNickname;
  ss >> clientNickname;
  std::string command;
  ss >> command;
  std::string arg;
  std::getline(ss >> std::ws, arg);

#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "Client: " << clientNickname << " Command: " << command
        << " Arg: " << arg;
    Server::printLog(DEBUG_LOG, BOT_L, oss.str());
  }
#endif
}

/*============================================================================*/
/*       API Servers responses handling                                       */
/*============================================================================*/

void Bot::handleResponse(void) {
  Server::printLog(INFO_LOG, BOT_L, "Handling API response");
}

/*============================================================================*/
/*       Helper functions                                                     */
/*============================================================================*/

