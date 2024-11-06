/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/06 13:02:34 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_SERVER_HPP_
#define INCLUDES_SERVER_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../includes/Config.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include "../includes/numericReplies.hpp"

#define SRV_NAME "ircserv.localhost"
#define SRV_VERSION "1.0.0"

typedef std::map<int, Client> clientsMap;
typedef std::map<std::string, Channel> channelsMap;

extern Config *gConfig;

enum Command {
  JOIN,
  KICK,
  INVITE,
  TOPIC,
  MODE,
  LIST,
  NOTICE,
  NICK,
  PRIVMSG,
  QUIT,
  PING,
  CAP,
  USER,
  PASS,
  UNKNOWN
};

class Server {
 private:
  static bool                _signal;
  int                        _socketFd;
  int                        _port;
  std::string                _startTime;
  std::string                _password;
  clientsMap                 _clients;
  struct sockaddr_in         _address;
  std::vector<struct pollfd> _pollFds;
  channelsMap                _channels;

 public:
  explicit Server(int port, std::string password);

  /* Getters */
  int getSocketFd(void) const;
  int getPort(void) const;
  const std::string &getPassword(void) const;
  const Client &getClientByFd(int fd) const;
  Channel &getChannelByName(const std::string &name);

  /* Server Mounting */
  void runServer(void);
  void createSocket(void);
  void createPoll(void);
  void fetchStartTime(void);
  void monitorConnections();
  static void signalHandler(int signal);

  /* Clients Management */
  void acceptNewClient();
  void sendConnectionMessage(const Client &client) const;
  void receiveMessage(int fd);

  /* Clients message handling */
  void handleInitialMessage(Client *client, const std::string &msg);
  void handleOtherMessage(const Client &client, const std::string &msg);
  void handleClientMessage(int fd);

  /* Clear and Close */

  void closeServer(void);
  void clearClient(int fd);
  void closeClient(int fd);

  /* Commands handling */
  void handleCommand(const std::string &command,
                     const std::string &argument, int fd);
  void sendToAllClients(const std::string &message);  // Broadcast

  /*-------- QUIT --------*/
  void quit(const std::string &argument, Client *client, clientsMap *cltMap);

  /*-------- JOIN --------*/
  // void joinChannel(std::string &channelName, int fd);

  /*-------- KICK --------*/

  /*-------- INVITE --------*/

  /*-------- TOPIC --------*/

  /*-------- MODE --------*/

  /*-------- LIST --------*/

  /*-------- NOTICE --------*/

  /*-------- PRIVMSG --------*/

  /*-------- PING --------*/
  void ping(Client *client, const std::string &token);
};

#endif  // INCLUDES_SERVER_HPP_
