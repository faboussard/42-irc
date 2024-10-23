/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/17 11:53:10 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_SERVER_HPP_
#define INCLUDES_SERVER_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"

typedef std::map<int, Client> clientsMap;
typedef std::map<std::string, Channel> channelsMap;
typedef Client &ClientRef; // Type de référence pour les clients

class Server {
private:
  static bool _signal;
  int _socketFd;
  int _port;
  clientsMap _clients;
  struct sockaddr_in _address;
  std::vector<struct pollfd> _pollFds;
  channelsMap _channels;

public:
  explicit Server(int port);
  ClientRef getClientByFd(int fd);

  /* Server Mounting */
  void runServer();
  void createSocket();
  void createPoll();
  void monitorConnections();
  static void signalHandler(int signal);

  /* Clients Management */

  void acceptNewClient();
  void receiveMessage(int fd);
  void handleClientMessage(int fd);

  /* Clear and Close */

  void closeServer();
  void clearClient(int fd);
  void closeClient(int fd);

  /* Chat Commands */
  void handleCommand(const std::string &command, int fd);

  // for channel, list, ","
  void sendToAllClients(const std::string &message);
};

#endif // INCLUDES_SERVER_HPP_
