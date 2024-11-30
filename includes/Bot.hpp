/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/30 15:23:50 by yusengok         ###   ########.fr       */
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
#define BOT_NICK "KawaiiBot"
#define BOT_USER "kawaiiBot 0 * :kawaiiBot"
#define PING_MSG "PING ft_irc\r\n"
#define PONG_MSG "PONG ft_irc\r\n"

#define JOKEAPI_HOST "icanhazdadjoke.com"
#define INSULTMEAPI_HOST "evilinsult.com"
#define ADVICEAPI_HOST "api.adviceslip.com"
#define WEATHERAPI_HOST "weatherapi.com"

#define CURL "curl -s "  // -s for silent mode
// #define JOKE_URL "https://icanhazdadjoke.com/"
#define INSULTME_URL \
  "https://evilinsult.com/generate_insult.php?lang=en&type=json"
#define ADVICE_URL "https://api.adviceslip.com/advice"
// #define WEATHER_URL "wttr.in/Paris\?n"
// curl 'wttr.in/Tokyo?format="%l:+%C'
// curl 'wttr.in/Lyon?format=j1'
#define WEATHER_URL1 "api.weatherapi.com/v1/forecast.json?key="
#define WEATHER_URL2 "&q=lyon&days=3"
#define JOKE_URL "https://httpbin.org/delay/50"  // Timeout check 

struct BotRequest {
  std::string clientNickname;
  eBotCommand command;
  // std::string arg;

  int fdForApi;
  FILE *fpForApi;
  int timeoutInMs; 
  std::string apiResponse;

  BotRequest(const std::string &nick, eBotCommand command)
      : clientNickname(nick),
        command(command),
        fdForApi(-1),
        fpForApi(NULL),
        timeoutInMs(10000),
        apiResponse("") {}
};

class Bot {
 private:
  static bool _signal;
  const std::string _nick;
  const std::string _user;

  stringVector _instructions;
  stringVector _jokeCat;
  stringVector _adviceCat;
  stringVector _insultMeCat;
  stringVector _unknownCat;
  stringVector _timeoutCat;

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
  /* Constructor helpers */
   void constructInstruction(void);
   void constructAsciiCats(void);

  /* Bot launch */
  void createSocket(void);
  void connectToIrcServer(void);
  void listenToIrcServer(void);
  bool authenticate(void);
  bool waitForPassAuthentication(int timeLimitInMs);
  bool waitForNickValidation(int timeLimitInMs);
  bool waitForConnectionMessage(int timeLimitInMs);

  /* Requests handling */
  void handleServerMessage(void);
  std::string readMessageFromServer(void);
  bool sendMessageToServer(const std::string &message);
  BotRequest parseRequest(const std::string &requestBuffer);
  std::string parseResponseByKey(const std::string &response,
                                 const std::string &key);

  /* Commands handling */
  FILE *openCurl(BotRequest *request, const std::string &url);
  void sendAsciiCatByCommand(BotRequest *request, eBotCommand command);
  void unknownCommand(BotRequest *request);
  void sendAsciiCatTimeout(BotRequest *request);

  void joke(BotRequest *request);
  void insultMe(BotRequest *request);
  void advice(BotRequest *request);
  void weather(BotRequest *request);

  /* Responses handling */
  void handleApiResponse(int fd);
  void receiveResponseFromApi(std::deque<BotRequest>::iterator itRequest);
  void sendResponseToServer(std::deque<BotRequest>::iterator itRequest);

  /* Log */
  void logcreatSocketForApi(void);
  void logApiRequest(int fd, const std::string &apiHost);
  void logApiResponse(int fd);
  void logApiTimeout(int fd, eBotCommand command);
#ifdef DEBUG
  void debugLogServerMessageSplit(const std::string &clientNickname,
                                  const std::string &commandStr);
  void debugLogParsedMessage(BotRequest request);
  void debugLogWaitingRequests(void);
#endif
};

//MENU
#define BOT_MENU1 "  /\\_/\\"
#define BOT_MENU2 " ( o.o )"
#define BOT_MENU3 \
  "─ U───U────────────────────────────────────────────────────────"
#define BOT_MENU4 "         Hello! I'm KawaiiBot, what can I do for you?"
#define BOT_MENU5 "────────────────────────────────────────────────────────── ♥ ──"
#define BOT_MENU6 "🤣 Feeling down? I'll lift you up with a dad joke. 👉!JOKE"
#define BOT_MENU7 "😈 Craving some sass? I can roast you. 👉!INSULTME"
#define BOT_MENU8 "👼 Need guidance? Let me share some wisdom with you. 👉!ADVICE"
#define BOT_MENU9 "🌤️ Wondering about the weather? Ask away. 👉!WEATHER <city name>"
#define BOT_MENU10 "🎲 Bored? Let's spice it up with something fun. 👉!RANDOM"
#define BOT_MENU {BOT_MENU1, BOT_MENU2, BOT_MENU3, BOT_MENU4, BOT_MENU5, BOT_MENU6, BOT_MENU7, BOT_MENU8, BOT_MENU9, BOT_MENU10}

// JOKE
#define JOKE_CAT_1 "     /\\_/\\"
#define JOKE_CAT_2 "    (• o •)    Haha! What a joke!"
#define JOKE_CAT_3 "     > ^ < 🐾"
#define JOKE_CAT {JOKE_CAT_1, JOKE_CAT_2, JOKE_CAT_3}

// ADVICE
#define ADVICE_CAT_1 "     /\\_/\\"
#define ADVICE_CAT_2 "    ( o.o )   Here's some wisdom:"
#define ADVICE_CAT_3 "     > ^ <"
#define ADVICE_CAT_4 "     || ||       *meow*"
#define ADVICE_CAT {ADVICE_CAT_1, ADVICE_CAT_2, ADVICE_CAT_3, ADVICE_CAT_4}

// INSULTME
#define INSULTME_CAT_1 "     /\\_/\\"
#define INSULTME_CAT_2 "    (>_< )   That was uncalled for!"
#define INSULTME_CAT_3 "     > ^ <"
#define INSULTME_CAT_4 "    (    )/   🐾"
#define INSULTME_CAT {INSULTME_CAT_1, INSULTME_CAT_2, INSULTME_CAT_3, INSULTME_CAT_4}

// DEFAULT / UNKNOWN
#define DEFAULT_CAT_1 "     /\\_/\\"
#define DEFAULT_CAT_2 "    ( o.o )   I'm confused!"
#define DEFAULT_CAT_3 "     > ^ < 🐾"
#define DEFAULT_CAT_4 " Try !MENU for a list of commands!"
#define DEFAULT_CAT {DEFAULT_CAT_1, DEFAULT_CAT_2, DEFAULT_CAT_3, DEFAULT_CAT_4}

// TIMEOUT
#define TIMEOUT_CAT_1 "    /\\_/\\"
#define TIMEOUT_CAT_2 "   ( -_- )   Zzz... Sorry, temporarily unavailable."
#define TIMEOUT_CAT_3 "    > ^ <"
#define TIMEOUT_CAT {TIMEOUT_CAT_1, TIMEOUT_CAT_2, TIMEOUT_CAT_3}

#endif  // INCLUDES_BOT_HPP_
