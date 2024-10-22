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

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>

#include "../includes/Client.hpp"

class Client;

class Server {
 private:
  static bool _signal;
  int _socketFd;
  int _port;
  std::vector<Client> _clients;
  struct sockaddr_in _address;
  std::vector<struct pollfd> _pollFds;

 public:
  explicit Server(int port);

  static void signalHandler(int signal);

  void runServer();
  void closeServer();
  void createSocket();
  void clearClient(int fd);
  void acceptNewClient();
  void receiveMessage(int fd);
  void handleMessage(int fd, const std::string& message);
};

#endif  // INCLUDES_SERVER_HPP_
