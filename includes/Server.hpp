/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/26 16:13:00 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include <cctype>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// class Bot;
// struct BotRequest;

#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include "../includes/Config.hpp"
#include "../includes/Log.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/enums.hpp"

#define SRV_NAME "ircserv"
#define SRV_VERSION "1.0.0"

typedef std::map<int, Client> clientsMap;
typedef std::map<std::string, Channel> channelsMap;
typedef std::vector<std::string> stringVector;
typedef std::pair<std::vector<std::string>, std::vector<std::string> >
    StringVectorPair;

extern Config *gConfig;

#define BOT_NAME "ircbot"

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

  /* Server Mounting */
  void runServer(void);
  void createSocket(void);
  // void createPoll(void);
  static void signalHandler(int signal);
  void acceptAndChat(void);
  void closeServer(void);

  /*  Getters */
  // int getSocketFd() const;
  int getPort() const;
  // const std::string &getPassword() const;
  //  Client &findClientByFd(int fd);
  // const channelsMap &getChannels() const;
  // const clientsMap &getClients() const;

  /*  Finders */
  // Channel *findChannelByName(const std::string &name);
  Client *findClientByNickname(const std::string &nickname);

  /*  Log */
  static void printLog(eLogLevel level, eLogContext context,
                       const std::string &message);

 private:
  /* Server Management */
  void fetchStartTime(void);

  /* Clients Management */
  void acceptNewClient(void);
  void sendConnectionMessage(const Client &client) const;
  // void receiveMessage(int fd);

  /* Clients message handling */
  void handleInitialMessage(Client *client, const std::string &msg);
  void handleOtherMessage(const Client &client, const std::string &msg);
  void handleClientMessage(int fd);

  /* Other methods */
  // void sendToAllClients(const std::string &message);

  /* Clear and Close */
  void clearClient(int fd);
  void closeClient(int fd);

  /* Checkers */
  bool nickExists(const std::string &nick) const;
  bool channelExists(const std::string &channel);

  /* Commands handling */
  void handleCommand(Command command, const std::string &argument, int fd);
  void broadcastInChannel(const Client &client, const Channel &channel,
                          const std::string &command,
                          const std::string &content);
  void sendNotice(const Client &client, const std::string &message);

  /*  Command  */
  /*-------- JOIN --------*/
  bool isLeaveAllChannelsRequest(const std::string &param);
  bool isChannelNameValid(const std::string &channelToCheck,
                          const Client &client);

  void joinChannel(int fd, const std::string &param);

  void addChanneltoServer(const std::string &channelName);
  void sendJoinMessageToClient(const std::string &nick,
                               const std::string &channelName,
                               const Client &client);
  void processJoinRequest(int fd, Client *client, Channel *channel);
  bool isKeyValid(const Channel &channel, const std::string &keyToCheck,
                  const Client &client);
  bool isChannelNotFull(const Channel &channel, const Client &client);
  bool isClientAllowedInInviteOnlyChannel(const Channel &channel,
                                          const Client &client);
  StringVectorPair parsejoin(const std::string &channels,
                                                   const std::string &keys);

  /*-------- PART --------*/
  void quitAllChannels(int fd);
  void quitChannel(int fd, Channel *channel, Client *client);
  void sendPartMessageToClient(int fd, const std::string &nick,
                               const std::string &channelName);

  /*-------- QUIT --------*/
  void quit(const std::string &argument, Client *client, clientsMap *cltMap);

  /*-------- INVITE --------*/
  void invite(int fd, const std::string &arg);
  bool inviteParamsValid(int invitingClientFd, const std::string &invitedNick,
                         const std::string &channelNameWithPrefix);
  void inviteClientToChannel(int invitingClientFd,
                             const std::string &invitedNick,
                             const std::string &channelNameWithPrefix);
  void sendInvitedChannelsList(int fd) const;

  /*-------- TOPIC --------*/
  void topic(int fd, const std::string &arg);
  bool parseTopicParams(const std::string &arg, stringVector *params,
                        const Client &client);
  void sendTopic(const Client &client, const Channel &channel);
  void updateTopic(const Client &client, Channel *channel,
                   const std::string &newTopic);

  /*-------- MODE --------*/
  void mode(int fd, const std::string &arg);
  char checkModeArguments(const stringVector &modeStrings,
                             const stringVector &arguments);
  std::string checkModeString(const stringVector &argumentToCheck);

  bool isChannelValid(int fd, const std::string &channel);
  void switchMode(Client *client, const std::string &channelName,
                  const stringVector &modeStrings,
                  const stringVector &argumentVector);
  StringVectorPair parseMode(const std::string &arg);

  /*-------- WHO --------*/
  void who(const Client &client, const std::string &arg);
  void sendClientsListInChannel(const Client &client, const Channel &channel);

  /*-------- LIST --------*/
  void list(const Client &client, const std::string &argument);
  void listAllChannels(int fd, const std::string &nick);
  void listChannels(const stringVector &channels, const Client &client);

  /*-------- KICK --------*/
  void kick(int fd, const std::string &arg);
  void parseKickParams(const std::string &param, const Client &client,
                       std::string *channelName, std::string *targetNick,
                       std::string *reason);

  /*-------- PRIVMSG --------*/
  void privmsg(int fd, const std::string &arg);
  void broadcastToOperatorsOnly(const Client &sender, const Channel &channel,
                                const std::string &command,
                                const std::string &content);
  void sendPrivmsgToClient(const Client &sender, const Client &receiver,
                           const std::string &message);
  bool parsePrivmsgArguments(const std::string &arg, const Client &client,
                             std::vector<std::string> *targets,
                             std::string *message);

  bool validPrivmsgTargets(const std::string &arg, const Client &client,
                           stringVector *targets);

  /*-------- PING --------*/
  void ping(const Client &client, const std::string &token);

  /* Bot */
 private:
  Client *_bot;
  void botCommands(Client *client, Command command, const std::string &arg);
  void sendRequestToBot(const Client &client, Command command,
                        const std::string &arg);

//   void addBot(void);

//  public:


// /*---------- OLD VERSION -----------------------------------------------*/
//  private:
//   Bot *_bot;
//   std::vector<int> _botToApiSocketFds;  // --> will create Class BotRequest
//   std::queue<std::string> _responsesFromBot;

//  public:
//   // void set_botToApiSocketFds(std::vector<int> *botToApiSocketFds);
//   void addBotResponseToQueue(const std::string &response);
//   void addBotSocketFdToPoll(int newFd);
//   void removeApiSocketFdFromPoll(int soketFd);
//   const std::vector<int> &getBotToApiSocketFds(void) const;

//  private:
//   void addBotToPoll(int pipeFdServerToBot, int pipeFdBotToServer);
//   void sendBotInstruction(const Client &client);
//   void handleBotResponse(int serverFdListenBot);

  /* Tests */
  // void addClient(int fd, const Client &client);
};

#endif  // INCLUDES_SERVER_HPP_
