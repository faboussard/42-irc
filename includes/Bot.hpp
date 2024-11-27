/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/27 21:24:48 by yusengok         ###   ########.fr       */
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

#define LOCALHOST "127.0.0.1"
#define BOT_NICK "ircbot"
#define BOT_USER "ircbot 0 * :ircbot"

#define NUMBERSAPI_HOST "numbersapi.com"
#define JOKEAPI_HOST "icanhazdadjoke.com"
#define INSULTMEAPI_HOST "evilinsult.com"

#define NUMBERS_URL "http://numbersapi.com/"  // Need to add a number at the end
#define JOKE_URL "https://icanhazdadjoke.com/"
#define INSULTME_URL \
  "https://evilinsult.com/generate_insult.php?lang=en&type=json"

struct BotRequest {
  std::string clientNickname;
  Command command;
  std::string arg;

  int fdForApi;
  FILE *fpForApi;
  std::string apiResponse;

  BotRequest(const std::string &nick, Command command,
             const std::string &argument)
      : clientNickname(nick),
        command(command),
        arg(argument),
        fdForApi(-1),
        fpForApi(NULL),
        apiResponse("") {}
};

class Bot {
 private:
  static bool _signal;
  const std::string _nick;
  const std::string _user;
  bool _connectedToServer;

  stringVector _instructions;

  int _serverPort;
  const std::string _serverPass;

  /* Bot-IRC Server communication */
  int _botPort;
  int _botSocketFd;
  std::vector<struct pollfd> _botPollFds;
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
  bool checkServerConneciion(void);

  std::string readMessageFromServer(void);
  bool sendMessageToServer(const std::string &message);

  /* Requests handling */
  void handleServerMessage(void);  // receive, parse, send
  BotRequest parseRequest(const std::string &requestBuffer);
  // std::string parseRequest(const BotRequest &request);
  // void findApiInfo(BotRequest *request);
  // void sendRequestToApi(const std::string &request, int socketFd);

  /* Responses handling */
  void receiveResponseFromApi(std::deque<BotRequest>::iterator itRequest);
  std::string parseResponse(std::deque<BotRequest>::iterator itRequest);
  void sendResponseToServer(std::deque<BotRequest>::iterator itRequest);

  /* Commands handling */
  void bot(BotRequest *request);
  void numbers(BotRequest *request);
  void joke(BotRequest *request);
  void film(BotRequest *request);
  void insultMe(BotRequest *request);

  /* Log */
  void logcreatSocketForApi(void);
  void logApiRequest(int fd, const std::string &apiHost);
  void logApiResponse(int fd);
#ifdef DEBUG
  void debugLogReadRequest(BotRequest request);
  void debugLogWaitingRequests(void);
#endif
};

// #define BOT_RESPONSE_HEADER (std::string(":") + BOT_NICK + " PRIVMSG ")

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
