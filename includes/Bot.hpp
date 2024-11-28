/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/28 17:18:47 by faboussa         ###   ########.fr       */
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
#include <cstdio>
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

#define JOKEAPI_HOST "icanhazdadjoke.com"
#define INSULTMEAPI_HOST "evilinsult.com"
#define ADVICEAPI_HOST "api.adviceslip.com"
#define WEATHERAPI_HOST "weatherapi.com"

#define CURL "curl -s "  // -s for silent mode
#define JOKE_URL "https://icanhazdadjoke.com/"
#define INSULTME_URL \
  "https://evilinsult.com/generate_insult.php?lang=en&type=json"
#define ADVICE_URL "https://api.adviceslip.com/advice"
// #define WEATHER_URL "wttr.in/Paris\?n"
#define WEATHER_URL1 "\"https://api.weatherapi.com/v1/forecast.json?key="
#define WEATHER_URL2 "&q="
#define WEATHER_URL3 "&days=1&aqi=no&alerts=no\""

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

 private:
  /* Bot launch */
  void createSocket(void);
  void connectToIrcServer(void);
  void listenToIrcServer(void);
  bool authenticate(void);
  bool checkServerConneciion(void);

  /* Requests handling */
  void handleServerMessage(void);
  std::string readMessageFromServer(void);
  bool sendMessageToServer(const std::string &message);
  BotRequest parseRequest(const std::string &requestBuffer);
  std::string parseResponseByKey(const std::string &response,
                                 const std::string &key);

  /* Commands handling */
  void displayAsciiByCommand(BotRequest *request, eBotCommand command);
  void unknownCommand(BotRequest *request);

  /* Commands handling */
  FILE *openCurl(BotRequest *request, std::string url);
  void joke(BotRequest *request);
  void insultMe(BotRequest *request);
  void advice(BotRequest *request);

  /* Responses handling */
  void handleApiResponse(int fd);
  void receiveResponseFromApi(std::deque<BotRequest>::iterator itRequest);
  void sendResponseToServer(std::deque<BotRequest>::iterator itRequest);

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

//MENU
#define BOT_MENU1 "  /\\_/\\"
#define BOT_MENU2 " ( o.o )"
#define BOT_MENU3 \
  "─ U───U────────────────────────────────────────────────────────"
#define BOT_MENU4 "         Hello! I'm IRCbot, what can I do for you?"
#define BOT_MENU5 "────────────────────────────────────────────────────────── ♥ ──"
#define BOT_MENU6 "🤣 Feeling down? I'll lift you up with a dad joke. 👉!JOKE"
#define BOT_MENU7 "😈 Craving some sass? I can roast you. 👉!INSULTME"
#define BOT_MENU8 "👼 Need guidance? Let me share some wisdom with you. 👉!ADVICE"
#define BOT_MENU9 "🌤️ Wondering about the weather? Ask away. 👉!WEATHER <city name>"
#define BOT_MENU10 "🎲 Bored? Let's spice it up with something fun. 👉!RANDOM"

// JOKE
#define JOKE_CAT_1 "     /\\_/\\"
#define JOKE_CAT_2 "    (• o •)    Haha! What a joke!"
#define JOKE_CAT_3 "     > ^ < 🐾"

// ADVICE
#define ADVICE_CAT_1 "     /\\_/\\"
#define ADVICE_CAT_2 "    ( o.o )   Here's some wisdom:"
#define ADVICE_CAT_3 "     > ^ <"
#define ADVICE_CAT_4 "     || ||       *meow*"

// INSULTME
#define INSULTME_CAT_1 "     /\\_/\\"
#define INSULTME_CAT_2 "    (>_< )   That was uncalled for!"
#define INSULTME_CAT_3 "     > ^ <"
#define INSULTME_CAT_4 "    (    )/   🐾"

// DEFAULT / UNKNOWN
#define DEFAULT_CAT_1 "     /\\_/\\"
#define DEFAULT_CAT_2 "    ( o.o )   I'm confused!"
#define DEFAULT_CAT_3 "     > ^ < 🐾"
#define DEFAULT_CAT_4 " Try !MENU for a list of commands!"
#endif  // INCLUDES_BOT_HPP_
