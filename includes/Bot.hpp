/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/05 11:27:40 by yusengok         ###   ########.fr       */
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
#include <map>
#include <string>
#include <vector>

#include "../includes/colors.hpp"
#include "../includes/enums.hpp"
#include "../includes/utils.hpp"

#define ENV_FILE ".env"
#define LOCALHOST "127.0.0.1"

#define BOT_NICK "KawaiiBot"
#define BOT_USER "Kawaiibot 0 * :KawaiiBot"

#define JOKEAPI_HOST "icanhazdadjoke.com"
#define INSULTMEAPI_HOST "evilinsult.com"
#define ADVICEAPI_HOST "api.adviceslip.com"
#define WEATHERAPI_HOST "weatherapi.com"

#define CURL "curl -s "  // -s for silent mode
#define JOKE_URL "https://icanhazdadjoke.com/"
#define INSULTME_URL \
  "https://evilinsult.com/generate_insult.php?lang=en&type=json"
#define ADVICE_URL "https://api.adviceslip.com/advice"
#define WEATHER_URL1 "\"api.weatherapi.com/v1/forecast.json?days=2&key="
#define WEATHER_URL2 "&q="
#define QUOTE "\""
#define DEFAULT_CITY "Lyon"
#define SNOWY_CODES                                \
  {"1114", "1117", "1210", "1213", "1216", "1219", \
  "1222", "1225", "1255", "1258", "1279", "1282"}
// #define JOKE_URL "https://httpbin.org/delay/50"  // For timeout case test

#define ESCAPE_QUOT "&quot;"
#define ESCAPE_APOS "&apos;"
#define ESCAPE_AMP "&amp;"
#define ESCAPE_LT "&lt;"
#define ESCAPE_GT "&gt;"

struct BotRequest {
  std::string clientNickname;
  eBotCommand command;
  std::string commandArg;

  int fdForApi;
  FILE *fpForApi;
  int timeoutInMs;
  std::string apiResponse;

  BotRequest(const std::string &nick, eBotCommand command,
             const std::string &arg)
      : clientNickname(nick),
        command(command),
        commandArg(arg),
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

  int _serverPort;
  const std::string _serverPass;

  /* Bot-IRC Server communication */
  int _botSocketFd;
  std::vector<struct pollfd> _botPollFds;
  std::deque<BotRequest> _requestDatas;

  /* Ascii cats */
  stringVector _hello;
  stringVector _jokeCat;
  stringVector _adviceCat;
  stringVector _insultMeCat;
  stringVector _sunnyCat;
  stringVector _cloudyCat;
  stringVector _foggyCat;
  stringVector _rainyCat;
  stringVector _snowyCat;
  stringVector _thunderCat;
  stringVector _frostyCat;
  stringVector _unknownWeatherCat;
  stringVector _unknownCat;
  stringVector _timeoutCat;

  std::map<std::string, char> _htmlEscapes;

 public:
  explicit Bot(int serverPort, const std::string &serverPass);
  ~Bot(void);

  void runBot(void);
  static void signalHandler(int signal);

 private:
  /* Constructor helpers */
  void constructInstruction(void);
  void constructAsciiCats(void);
  void constructHtmlEscapes(void);

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
  eForecast parseJsonWeatherResponse(const std::string &apiResponse);

  /* Commands handling */
  FILE *openCurl(BotRequest *request, const std::string &url);
  void sendAsciiCatByCommand(BotRequest *request, eBotCommand command);
  void unknownCommand(BotRequest *request);
  void sendAsciiCatServiceUnavailable(BotRequest *request);
  void sendAsciiCatForecast(const std::string &nick, eForecast forecast);
  void sendAsciiCat(const std::string &nick, const stringVector &cat);

  void joke(BotRequest *request);
  void insultMe(BotRequest *request);
  void advice(BotRequest *request);
  void weather(BotRequest *request);

  /* API Responses handling */
  void handleApiResponse(int fd);
  void receiveResponseFromApi(std::deque<BotRequest>::iterator itRequest);
  std::string decodeHtmlEscapes(const std::string &str);
  void sendResponseToServer(std::deque<BotRequest>::iterator itRequest);

  /* Log */
  void logcreatSocketForApi(void);
  void logApiRequest(int fd, const std::string &apiHost);
  void logApiResponse(int fd);
  void logApiTimeout(int fd, eBotCommand command);
#ifdef DEBUG
  void debugLogServerMessageSplit(const std::string &clientNickname,
                                  const std::string &commandStr,
                                  const std::string &arg);
  void debugLogParsedMessage(BotRequest request);
  void debugLogWaitingRequests(void);
#endif
};

#endif  // INCLUDES_BOT_HPP_
