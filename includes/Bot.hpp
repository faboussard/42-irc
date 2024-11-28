/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/28 10:39:17 by yusengok         ###   ########.fr       */
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
#define PING_MSG "PING ft_irc\r\n"
#define PONG_MSG "PONG ft_irc\r\n"

#define NUMBERSAPI_HOST "numbersapi.com"
#define JOKEAPI_HOST "icanhazdadjoke.com"
#define INSULTMEAPI_HOST "evilinsult.com"

#define NUMBERS_URL "http://numbersapi.com/"  // Need to add a number at the end
#define JOKE_URL "https://icanhazdadjoke.com/"
#define INSULTME_URL \
  "https://evilinsult.com/generate_insult.php?lang=en&type=json"

struct BotRequest {
  std::string clientNickname;
  eBotCommand command;
  std::string arg;

  int fdForApi;
  FILE *fpForApi;
  std::string apiResponse;

  BotRequest(const std::string &nick, eBotCommand command,
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
  void handleServerMessage(void);  // receive, parse, send
  bool sendMessageToServer(const std::string &message);

  /* Requests handling */
  BotRequest parseRequest(const std::string &requestBuffer);
  void sendUnknownCommand(const BotRequest& newRequest);

  /* Responses handling */
  void receiveResponseFromApi(std::deque<BotRequest>::iterator itRequest);
  std::string parseResponse(std::deque<BotRequest>::iterator itRequest);
  void sendResponseToServer(std::deque<BotRequest>::iterator itRequest);

  /* Commands handling */
  void menu(BotRequest *request);
  void numbers(BotRequest *request);
  void joke(BotRequest *request);
  void insultMe(BotRequest *request);
  void advice(BotRequest *request);
  void randomCommand(BotRequest *request);

  /* Log */
  void logcreatSocketForApi(void);
  void logApiRequest(int fd, const std::string &apiHost);
  void logApiResponse(int fd);
#ifdef DEBUG
  void debugLogServerMessageSplit(const std::string &clientNickname,
                                     const std::string &commandStr,
                                     const std::string &arg);
  void debugLogParsedMessage(BotRequest request);
  void debugLogWaitingRequests(void);
#endif
};

#define BOT1 "  /\\_/\\"
#define BOT2 " ( o.o )"
#define BOT3 "─ U───U────────────────────────────────────────────────────────"
#define BOT4 "         Hello! I'm IRCbot, what can I do for you?"
#define BOT5 "────────────────────────────────────────────────────────── ♥ ──"
#define BOT6 " NUMBERS <number>: Ask me about a number, get a fun fact (∞ᴗ∞)"
#define BOT7 " JOKE: Feeling down? I'll lift you up with a dad joke (๑˃̵ᴗ˂̵)"
#define BOT8 " INSULTME: Craving some sass? I can roast you (•̀ᴗ•́)و ̑̑"
#define BOT9 " MOVIE <title>: Looking for a movie? Let me help you (｡•̀ᴗ-)✧"
#define BOT10 ""
#define BOT11 " RANDOM >> Bored? Let's spice it up with something fun (⊙_☉)"

#endif  // INCLUDES_BOT_HPP_
