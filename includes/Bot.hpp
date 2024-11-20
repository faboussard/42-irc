/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/20 08:22:23 by yusengok         ###   ########.fr       */
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

class Bot {
 private:
  int _serverFd;
  int _serverPort;

  int _botFd;
  int _botPort;
  struct sockaddr_in _address;
  std::vector<struct pollfd> _pollFds;

  void createSockets(void);
  void connectToServer(void);

  bool parseRequest(const std::string &request);
  void sendRequest(const std::string &request);
  void receiveResponse(void);
  bool parseResponse(const std::string &response);
  void sendResponse(const std::string &response);

 public:
  Bot(int serverPort);  //temporary
  ~Bot(void);  //temporary

  void runBot(void);
  void closeBot(void);

  
};

#endif  // INCLUDES_BOT_HPP_
