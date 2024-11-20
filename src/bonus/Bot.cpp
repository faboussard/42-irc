/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:01:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/20 14:13:08 by yusengok         ###   ########.fr       */
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
  pollSockets();
  _server->addBot(this);
}

void Bot::createSockets(void) {
  /* Creat socket for IRC Server */
  _ircSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_ircSocketFd == -1) {
    throw std::runtime_error("Failed to create socket");
  }
  int en = 1;
  if (setsockopt(_ircSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) {
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket");
  }
  if (fcntl(_ircSocketFd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket");
  }

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
}

void Bot::listenApiServer(void) {
  _apiAddress.sin_family = AF_INET;
  _apiAddress.sin_addr.s_addr = INADDR_ANY;
  _apiAddress.sin_port = htons(_apiPort);
  if (bind(_apiSocketFd, (struct sockaddr *)&_ircAddress, sizeof(_ircAddress)) == -1) {
    throw std::runtime_error(
        "Failed to bind socket. Port might be used elsewhere");
  }
  if (listen(_apiSocketFd, SOMAXCONN) == -1) {
    throw std::runtime_error("Failed to listen on socket");
  }
}

void Bot::connectToIrcServer(void) {
  _ircAddress.sin_family = AF_INET;
  _ircAddress.sin_addr.s_addr = INADDR_ANY;  // Need to set IRC server IP ?
  _ircAddress.sin_port = htons(_ircPort);
}

void Bot::pollSockets(void) {
  struct pollfd pollFds[2];
  pollFds[0].fd = _ircSocketFd;
  pollFds[0].events = POLLIN;
  pollFds[0].revents = 0;
  pollFds[1].fd = _apiSocketFd;
  pollFds[1].events = POLLIN;
  pollFds[1].revents = 0;

  // Add poll()
}
