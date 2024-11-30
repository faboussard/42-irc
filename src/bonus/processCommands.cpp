/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processCommands.cpp                                 :+:      :+:    :+: */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 10:31:27 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/30 22:26:56 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "../../includes/Bot.hpp"
#include "../../includes/Log.hpp"
#include "../../includes/utils.hpp"

/*============================================================================*/
/*       Send API request                                                     */
/*============================================================================*/

FILE *Bot::openCurl(BotRequest *request, const std::string &url) {
  std::cout << "openCurl---> " << url << std::endl;
  std::string curlCommand = CURL;
  curlCommand += url;
#ifdef DEBUG
  Log::printLog(DEBUG_LOG, BOT_L, "Curl command: " + curlCommand);
#endif
  FILE *fp = popen(curlCommand.c_str(), "r");
  if (fp != NULL) {
    request->fpForApi = fp;
    request->fdForApi = fileno(fp);
    _requestDatas.push_back(*request);

    struct pollfd newPoll;
    newPoll.fd = request->fdForApi;
    newPoll.events = POLLIN;
    newPoll.revents = 0;
    _botPollFds.push_back(newPoll);
  }
  return (fp);
}

/*============================================================================*/
/*       Regular commands                                                     */
/*============================================================================*/

void Bot::joke(BotRequest *request) {
  openCurl(request, JOKE_URL);
  logApiRequest(request->fdForApi, JOKEAPI_HOST);
}

void Bot::insultMe(BotRequest *request) {
  openCurl(request, INSULTME_URL);
  logApiRequest(request->fdForApi, INSULTMEAPI_HOST);
}

void Bot::advice(BotRequest *request) {
  openCurl(request, ADVICE_URL);
  logApiRequest(request->fdForApi, ADVICEAPI_HOST);
}

void Bot::weather(BotRequest *request) {
  const char* key = getenv("WEATHER_API_KEY");
  if (key == NULL) {
    Log::printLog(ERROR_LOG, BOT_L, "Weather API key not found");
    sendAsciiCatServiceUnavailable(request);
    return;
  }
  std::ostringstream oss;
  oss << WEATHER_URL1 << key << WEATHER_URL2;
  std::cout << oss.str() << std::endl;
  openCurl(request, "\"" + oss.str() + "\"");
  logApiRequest(request->fdForApi, WEATHERAPI_HOST);
}

// #define WEATHER_URL1 "api.weatherapi.com/v1/forecast.json?key="
// #define WEATHER_URL2 "&q=lyon&days=3"

// #void Bot::weather(BotRequest *request) {
// #  // std::string key = getenv("WEATHER_API_KEY")
// #  std::string city = "lyon";
  
// #  std::ostringstream oss;
// #  oss << std::string(WEATHER_URL1) << std::string(key.c_str()) << std::string(WEATHER_URL2) << std::string(city.c_str()) << std::string(WEATHER_URL3);
// #  std::cout << oss.str() << std::endl;
// #  openCurl(request, "\"" + oss.str() + "\"");
// #  // Set timeout for API response ?
// #  logApiRequest(request->fdForApi, WEATHERAPI_HOST);
// #}

// #define WEATHER_URL2 "&q="
// #define WEATHER_URL3 "&days=3"

/*============================================================================*/
/*       Errors                                                               */
/*============================================================================*/

void Bot::unknownCommand(BotRequest *request) {
  std::ostringstream oss;
  oss << "PRIVMSG " << request->clientNickname
      << " :Oops! Not sure what you're asking for?\r\n";
  sendMessageToServer(oss.str());
  sendAsciiCatByCommand(request, UNKNOWN_BOT_COMMAND);
}

void Bot::sendAsciiCatServiceUnavailable(BotRequest *request) {
  stringVector::iterator itEnd = _timeoutCat.end();
  for (stringVector::iterator it = _timeoutCat.begin(); it != itEnd ; ++it) {
    std::ostringstream oss;
    oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
    sendMessageToServer(oss.str());
  }
}
