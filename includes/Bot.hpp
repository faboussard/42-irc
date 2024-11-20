/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/20 22:27:21 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_BOT_HPP_
#define INCLUDES_BOT_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <vector>

#define BOT_NAME
#define IRC_PORT 6668
#define IRC_PORT2 6669
#define API_PORT 8080
#define API_PORT2 8008
#define MAX_CLIENTS_BOT 20
#define LOCALHOST "127.0.0.1"

#define BOT_CONNECTION_FAILED_IRC "Failed to connect to IRC server"

class Server;

class Bot {
 private:
  Server *_server;

  int _ircPort;
  int _ircSocketFd;
  struct sockaddr_in _ircAddress;

  int _apiPort;
  int _apiSocketFd;
  struct sockaddr_in _apiAddress;

  std::vector<struct pollfd> _pollFds;

  /* Bot launch */
  void createSockets(void);
  void connectToIrcServer(void);
  void listenApiServer(void);

  /* HTTP requests */
  void handleRequest(void);  // receive, parse, send
  bool parseRequest(const std::string &request);
  void sendRequest(const std::string &request);

  /* Replies */
  void handleResponse(void);  // receive, parse, send
  bool parseResponse(const std::string &response);
  void sendResponse(const std::string &response);

 public:
  struct pollfd pollFds[2];

  Bot(Server *server);
  ~Bot(void);

  void runBot(void);
  // void pollSockets(void);
  void closeBot(void);

  int getIrcSocketFd(void) const;
  int getApiSocketFd(void) const;
};

#endif  // INCLUDES_BOT_HPP_
