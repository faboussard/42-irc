/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/26 17:01:32 by yusengok         ###   ########.fr       */
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

#include <algorithm>
#include <deque>
#include <string>
#include <vector>

#include "../includes/colors.hpp"
#include "../includes/enums.hpp"
#include "../includes/utils.hpp"

#define BOT_NAME "ircbot"
// #define MAX_CLIENTS_BOT 20
#define LOCALHOST "127.0.0.1"

#define BOT_SOCKET_PORT 6668
#define BOT_SOCKET_PORT2 6669
#define BOT_NICK "ircbot"
#define BOT_USER "ircbot 0 * :ircbot"

#define NUMBERS_URL "http://numbersapi.com/" curl
#define JOKE_URL "https://icanhazdadjoke.com/"

class Server;

struct BotRequest {
  std::string clientNickname;
  Command command;
  std::string arg;

  std::string apiResponse;

  BotRequest(const std::string &nick, Command command,
             const std::string &argument)
      : clientNickname(nick),
        command(command),
        arg(argument),
        apiResponse(""){}
};

class Bot;

struct ThreadData {
  Bot* bot;
  std::string requestBuffer;
};

class Bot {
 private:
  static bool _signal;
  const std::string _nick;
  const std::string _user;

  stringVector _instructions;

  int _serverPort;
  const std::string _serverPass;

  // struct sockaddr_in _botAddress;
  int _botPort;
  int _botSocketFd;

  /* Bot-IRC Server & Bot-API Servers communication */
  Server *_server;
  int _pipeServerToBot[2];
  int _pipeBotToServer[2];
  std::deque<BotRequest> _requestDatas;

 public:
  explicit Bot(int serverPort, const std::string &serverPass, int botPort);
  ~Bot(void);

  void runBot(void);
  static void signalHandler(int signal);

  /* Getters */
  // int getServerToBotPipe0(void) const;
  // int getBotToServerPipe0(void) const;
  // const stringVector &getInstructions(void) const;

  /* Bot - IRC Server communication */
  // void receiveRequestInQueue(BotRequest newRequest);

  /* Bot - API Servers communication */
  void handleApiResponse(int fd);  // receive, parse, send

 private:
  /* Bot launch */
  void createSocket(void);
  void connectToIrcServer(void);
  void listenToIrcServer(void);
  bool authenticate(void);

  std::string readMessageFromServer(void);
  bool sendMessageToServer(const std::string &message);

  /* Requests handling */
  void handleRequest(void);  // receive, parse, send
  // BotRequest readRequest(void);
  std::string parseRequest(const BotRequest &request);
  // void findApiInfo(BotRequest *request);
  void sendRequestToApi(const std::string &request, int socketFd);

  /* Responses handling */
  std::string receiveResponseFromApi(
      int fd, std::deque<BotRequest>::iterator itRequest);
  bool parseResponse(const std::string &response);
  void sendResponseToServer(const std::string &response);

  /* Log */
  void logcreatSocketForApi(void);
  void logApiResponse(int fd);
  void logApiConnectionClosed(int fd);
#ifdef DEBUG
  void debugLogPipe(int ServerToBot0, int ServerToBot1, int BotToServer0,
                    int BotToServer1);
  void debugLogReadRequest(BotRequest request);
  void debugLogWaitingRequests(void);
#endif
};

#define BOT_RESPONSE_HEADER (std::string(":") + BOT_NAME + " PRIVMSG ")

#define BOT1 "  /\\_/\\\n"
#define BOT2 " ( o.o )\n"
#define BOT3 "─ U───U────────────────────────────────────────────────────────\n"
#define BOT4 "         Hello! I'm IRCbot, what can I do for you?"
#define BOT5 "────────────────────────────────────────────────────────── ♥ ──\n"
#define BOT6 "🌤️ WEATHER 🌤️ Ask me for a forecast, I'll bring you the skies.\n"
#define BOT7 "🌐 TRANSLATE 🌐 Lost in translation? I'm multilingual !\n"
#define BOT8 "🎨 ASCII ART 🎨 \n"
#define BOT9 "How to use ?\n"
// "WEATHER → Get weather updates.\n"
// "TRANSLATE <text> → Translate words in a snap.\n"
// "ASCIIART <topic> → Create ASCII art magic.\n")

#endif  // INCLUDES_BOT_HPP_
