/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/22 21:45:31 by yusengok         ###   ########.fr       */
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
#define API_PORT 8080
#define API_PORT2 8008
#define MAX_CLIENTS_BOT 20
#define LOCALHOST "127.0.0.1"

class Server;

struct BotRequest {
  std::string clientNickname;
  std::string command;
  std::string arg;

  BotRequest(const std::string& nick, const std::string& cmd,
             const std::string& argument)
        : clientNickname(nick), command(cmd), arg(argument) {}
};

class Bot {
 private:
  std::string _name;
  stringVector _instructions;

  /* Bot - IRC Server communication */
  Server *_server;
  int _pipeServerToBot[2];
  int _pipeBotToServer[2];
  std::queue<std::string> _requests;

  /* Bot - API Servers communication */
  int _apiPort;
  int _apiSocketFd;
  struct sockaddr_in _apiAddress;

 public:
  explicit Bot(Server *server);
  ~Bot(void);

  void runBot(void);

  /* Getters */
  int getServerToBotPipe0(void) const;
  int getBotToServerPipe0(void) const;
  int getApiSocketFd(void) const;
  const stringVector &getInstructions(void) const;

  /* Bot - IRC Server communication */
  void receiveRequestInQueue(const std::string &request);
  void handleRequest(void);  // receive, parse, send

  /* Bot - API Servers communication */
  void handleApiResponse(void);  // receive, parse, send

  static std::string botCommandStr(Command command);

 private:
  /* Bot launch */
  void createSocketForApi(void);
  void listenApiServer(void);

  /* Requests handling */
  BotRequest readRequest(void);
  std::string parseRequest(const BotRequest &request);
  void sendRequestToApi(const std::string &request);

  /* Responses handling */
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

#endif  // INCLUDES_BOT_HPP_
