/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/27 15:03:25 by fanny            ###   ########.fr       */
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

#include "../includes/types.hpp"
#include "../includes/utils.hpp"

#define BOT_NAME "ircbot"
// #define MAX_CLIENTS_BOT 20
#define LOCALHOST "127.0.0.1"

#define NUMBERS_HOST "numbersapi.com"
#define NUMBERS_PORT 80
#define JOKE_HOST "icanhazdadjoke.com"
#define JOKE_PORT 443

#define NUMBERS_URL "http://numbersapi.com/"
#define JOKE_URL "https://icanhazdadjoke.com/"

class Server;

struct BotRequest {
  std::string clientNickname;
  Command command;
  std::string arg;

  std::string ApiString;

  int socketFd;
  std::string apiHost;
  int apiPort;

  BotRequest(const std::string &nick, Command command,
             const std::string &argument)
      : clientNickname(nick),
        command(command),
        arg(argument),
        socketFd(-1),
        apiHost(""),
        apiPort(0) {}
};

class Bot {
 private:
  std::string _name;
  stringVector _instructions;

  /* Bot-IRC Server & Bot-API Servers communication */
  Server *_server;
  int _pipeServerToBot[2];
  int _pipeBotToServer[2];
  std::deque<BotRequest> _requestDatas;

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
  std::string receiveResponseFromApi(
      int fd, std::deque<BotRequest>::iterator itRequest);
  void sendResponseToServer(const std::string &nickname, const std::string &response);
std::string parseResponse(const std::string &response);
std::string parseNumbersResponse(const std::string &response);

  /* Log */
  void logcreatSocketForApi(const BotRequest &request);
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
#define BOT7 "💡 ADVICE 💡 Need some wisdom? Ask me for advice!\n"
#define BOT8 "😂 JOKE 😂 Ready for a laugh? Ask me for a joke!\n"
#define BOT9 "🚫 INSULT 🚫 Feeling brave? Ask me for an insult!\n"
#define BOT10 "🌐 TRANSLATE 🌐 Lost in translation? I'm multilingual!\n"
#define BOT11 "🎨 ASCII ART 🎨 Want some art? Ask me for ASCII art!\n"
#define BOT12 "📜 QUOTE 📜 Need inspiration? Ask me for a quote!\n"
#define BOT13 "🔍 FACT 🔍 Want to learn something new? Ask me for a fact!\n"
#define BOT14 "🎲 RANDOM 🎲 Feeling lucky? Ask me for something random!\n"
#define BOT15 "📅 DATE 📅 Curious about today's date? Ask me!\n"
#define BOT16 "🕒 TIME 🕒 Want to know the time? Just ask!\n"
#define BOT17 "how to use ? \n"
#define BOT18 "just type / and the command you want to use\n"

#endif  // INCLUDES_BOT_HPP_
