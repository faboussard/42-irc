/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/08 15:39:32 by faboussa         ###   ########.fr       */
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

#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include "../includes/Config.hpp"
#include "../includes/numericReplies.hpp"

#define SRV_NAME "ircserv.localhost"
#define SRV_VERSION "1.0.0"

typedef std::map<int, Client> clientsMap;
typedef std::map<std::string, Channel> channelsMap;
typedef std::vector<std::string> stringVector;

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
  WHO,
  UNKNOWN
};

class Server {
 private:
  static bool _signal;
  int _socketFd;
  int _port;
  std::string _startTime;
  std::string _password;
  clientsMap _clients;
  struct sockaddr_in _address;
  std::vector<struct pollfd> _pollFds;
  channelsMap _channels;

 public:
  explicit Server(int port, const std::string &password);

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

  int getSocketFd() const;
  int getPort() const;
  const std::string &getPassword() const;
  Client &getClientByFd(int fd);
  const Channel &getChannelByName(const std::string &name) const;
  const channelsMap &getChannels() const;
  const clientsMap &getClients() const;

  /*============================================================================*/
/*       Member functions                                                     */
/*============================================================================*/

  /* Clients Management */
  void acceptNewClient(void);
  void sendConnectionMessage(const Client &client) const;
  void receiveMessage(int fd);

  /* Clients message handling */
  void handleInitialMessage(Client *client, const std::string &msg);
  void handleOtherMessage(const Client &client, const std::string &msg);
  void handleClientMessage(int fd);

  /* Server Mounting */
  void runServer(void);
  void createSocket(void);
  void createPoll(void);
  void fetchStartTime(void);
  static void signalHandler(int signal);
  void acceptAndChat(void);

  /* Other methods */
  void sendToAllClients(const std::string &message);
  void handlePassword(int fd);

  /* Clear and Close */
  void closeServer(void);
  void clearClient(int fd);
  void closeClient(int fd);

  /* Commands handling */
  void handleCommand(const std::string &command, const std::string &argument,
                     int fd);

/*============================================================================*/
/*      Commands                                                              */
/*============================================================================*/

  /*-------- JOIN --------*/

  bool isValidChannelPrefix(const std::string &param);
  bool isValidChannelNameLength(const std::string &param);
  bool isLeaveAllChannelsRequest(const std::string &param);
  bool isChannelValid(const std::string &param, const Client &client);

  void joinChannel(const std::string &param, int fd);
  void sendJoinMessage(int fd, const Client &client,
                         const std::string &channelName);
  void addChanneltoServerIfNoExist(const std::string &channelName);
  void sendJoinMessageToClient(int fd, const std::string &nick,
                               const std::string &channelName,
                               const Client &client);
  void broadcastJoinMessage(int fd, const std::string &nick,
                            const std::string &channelName);
  void processJoinRequest(int fd, Client *client, const std::string &channelName, const std::vector<std::string> &keys, size_t i);
  void handlePartRequest(int fd, const std::string &param);
  void handleKey(Client *client, const Channel &channel, const std::string &key);
void checkChannelsCorrectness(const stringVector &channels);
void checkKeysCorrectness(const stringVector &keys);
  /*-------- PART --------*/
  
  void quitChannel(int fd);
  void broadcastPartMessage(int fd, const std::string &nick, const std::string &channelName);
  void sendPartMessageToClient(int fd, const std::string &nick, const std::string &channelName);
  /*-------- QUIT --------*/
  void quit(const std::string &argument, Client *client, clientsMap *cltMap);

  /*-------- JOIN --------*/

  /*-------- KICK --------*/
  void kick(int fd, const std::string &arg);

  /*-------- INVITE --------*/
  void invite(int fd, const std::string &arg);

  /*-------- TOPIC --------*/
  void topic(int fd, const std::string &arg);

  /*-------- MODE --------*/
  void mode(int fd, const std::string &arg);

  /*-------- WHO --------*/
  void who(const Client &client, const std::string &arg);
  void sendClientsListInChannel(const Client &client, const Channel &channel);

  /*-------- LIST --------*/
  void list(const Client &client, const std::string &argument);
  void listAllChannels(int fd, const std::string &nick);
  void listChannels(const stringVector &channels, const Client &client);
  bool findChannel(const std::string &channel);

  /*-------- NOTICE --------*/
  void notice(int fd, const std::string &arg);

  /*-------- PRIVMSG --------*/
  void privmsg(int fd, const std::string &arg);

  /*-------- PING --------*/
  void ping(Client *client, const std::string &token);
};

#endif  // INCLUDES_SERVER_HPP_
