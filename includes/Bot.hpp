/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/23 22:49:20 by yusengok         ###   ########.fr       */
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

#include <queue>
#include <string>
#include <vector>

#include "../includes/types.hpp"
#include "../includes/utils.hpp"

#define BOT_NAME "ircbot"
#define MAX_CLIENTS_BOT 20
#define LOCALHOST "127.0.0.1"

#define NUMBERS_HOST "numbersapi.com"
#define NUMBERS_PORT 80
#define JOKE_HOST "v2.jokeapi.dev"
#define JOKE_PORT 443

class Server;

struct BotRequest {
  std::string clientNickname;
  Command command;
  std::string arg;

  int socketFd;
  std::string apiHost;
  int apiPort;

  BotRequest(const std::string& nick, Command command,
             const std::string& argument)
        : clientNickname(nick), command(command), arg(argument),
          socketFd(-1), apiHost(""), apiPort(0) {}
};

class Bot {
 private:
  std::string _name;
  stringVector _instructions;

  /* Bot-IRC Server & Bot-API Servers communication */
  Server *_server;
  int _pipeServerToBot[2];
  int _pipeBotToServer[2];
  std::queue<BotRequest> _requestDatas;

 public:
  explicit Bot(Server *server);
  ~Bot(void);

  void runBot(void);

  /* Getters */
  int getServerToBotPipe0(void) const;
  int getBotToServerPipe0(void) const;
  const stringVector &getInstructions(void) const;

  /* Bot - IRC Server communication */
  void receiveRequestInQueue(BotRequest newRequest);
  void handleRequest(void);  // receive, parse, send

  /* Bot - API Servers communication */
  void handleApiResponse(int fd);  // receive, parse, send

  static std::string botCommandStr(Command command);

 private:
  /* Bot launch */
  void createSocketForApi(BotRequest *request);
  bool connectToApiServer(BotRequest *request);

  /* Requests handling */
  // BotRequest readRequest(void);
  std::string parseRequest(const BotRequest &request);
  void findApiInfo(BotRequest *request);
  void sendRequestToApi(const std::string &request, int socketFd);

  /* Responses handling */
  std::string receiveResponseFromApi(int fd);
  bool parseResponse(const std::string &response);
  void sendResponseToServer(const std::string &response);
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

/* Log */
void logcreatSocketForApi(const BotRequest &request);
void logApiResponse(int fd);
#ifdef DEBUG
void debugLogPipe(int ServerToBot0, int ServerToBot1, int BotToServer0,
                         int BotToServer1);
void debugLogReadRequest(BotRequest request);
#endif

#endif  // INCLUDES_BOT_HPP_
