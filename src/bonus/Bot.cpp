/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:01:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/20 22:27:02 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

#include "../../includes/Server.hpp"

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Bot::Bot(Server *server)
    : _server(server),
      _ircPort(IRC_PORT),
      _ircSocketFd(-1),
      _apiPort(API_PORT),
      _apiSocketFd(-1) {}

Bot::~Bot(void) { closeBot(); }

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

int Bot::getIrcSocketFd(void) const { return _ircSocketFd; }

int Bot::getApiSocketFd(void) const { return _apiSocketFd; }

/*============================================================================*/
/*       Setters                                                              */
/*============================================================================*/

/*============================================================================*/
/*       Bot launch                                                           */
/*============================================================================*/

void Bot::runBot(void) {
  if (_server->getPort() == IRC_PORT) _ircPort = IRC_PORT2;
  if (_server->getPort() == API_PORT) _apiPort = API_PORT2;
  createSockets();
  connectToIrcServer();
  listenApiServer();
  // pollFds[0].fd = _ircSocketFd;
  // pollFds[0].events = POLLIN;
  // pollFds[0].revents = 0;
  // pollFds[1].fd = _apiSocketFd;
  // pollFds[1].events = POLLIN;
  // pollFds[1].revents = 0;
  _server->addBot(this);
}

void Bot::createSockets(void) {
  /* Creat socket for IRC Server */
  _ircSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_ircSocketFd == -1)
    throw std::runtime_error("Failed to create socket");

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
  oss << "Socket for IRC server created at Port " << _ircPort << " (fd" << _ircSocketFd << ")";
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif

  /* Creat socket for API Server */
  _apiSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_apiSocketFd == -1) {
    throw std::runtime_error("Failed to create socket");
  }
  en = 1;
  if (setsockopt(_apiSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) {
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket");
  }
  if (fcntl(_apiSocketFd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket");
  }
#ifdef DEBUG
{
  std::ostringstream oss;
  oss << "Socket for API server created at Port " << _apiPort << " (fd" << _apiSocketFd << ")";
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif
}

void Bot::listenApiServer(void) {
  _apiAddress.sin_family = AF_INET;
  _apiAddress.sin_addr.s_addr = INADDR_ANY;
  _apiAddress.sin_port = htons(_apiPort);
  if (bind(_apiSocketFd, (struct sockaddr *)&_apiAddress, sizeof(_apiAddress)) == -1) {
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
  std::ostringstream oss;
  oss << "Attempting to connect to IRC server at " LOCALHOST << ":"
      << _server->getPort();
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
#endif

  if (connect(_ircSocketFd, (struct sockaddr *)&_ircAddress, sizeof(_ircAddress)) == -1) {
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

// void Bot::pollSockets(void) {
//   // struct pollfd pollFds[2];
//   // pollFds[0].fd = _ircSocketFd;
//   // pollFds[0].events = POLLIN;
//   // pollFds[0].revents = 0;
//   // pollFds[1].fd = _apiSocketFd;
//   // pollFds[1].events = POLLIN;
//   // pollFds[1].revents = 0;

//   // while (Server::_signal == false) {
//     int pollRet = poll(pollFds, 2, -1);
//     if (pollRet == -1) {
//       Server::printLog(NOTIFY_LOG, BOT_L, "Connection closed");
//       return;
//     }
//     if (pollFds[0].revents & POLLIN && Server::_signal == false)
//       handleRequest();
//     if (pollFds[1].revents & POLLIN && Server::_signal == false)
//       handleResponse();
//   // }
// }

/*============================================================================*/
/*       Bot shut down                                                        */
/*============================================================================*/

void Bot::closeBot(void) {
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

/*============================================================================*/
/*       Client requests handling                                             */
/*============================================================================*/

void Bot::handleRequest(void) {
  Server::printLog(INFO_LOG, BOT_L, "Handling request");
}

/*============================================================================*/
/*       API Servers responses handling                                       */
/*============================================================================*/

void Bot::handleResponse(void) {
  Server::printLog(INFO_LOG, BOT_L, "Handling API response");
}
