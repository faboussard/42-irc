/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processCommands.cpp                                 :+:      :+:    :+: */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 10:31:27 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/03 09:57:00 by yusengok         ###   ########.fr       */
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
#include <string>

#include "../../includes/Bot.hpp"
#include "../../includes/Log.hpp"
#include "../../includes/utils.hpp"

/*============================================================================*/
/*       Send API request                                                     */
/*============================================================================*/

FILE *Bot::openCurl(BotRequest *request, const std::string &url) {
  std::string curlCommand = CURL;
  curlCommand += url;
  Log::printLog(DEBUG_LOG, BOT_L, "Curl command: " + curlCommand);
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
  const char *key = getenv("WEATHER_API_KEY");
  if (key == NULL) {
    Log::printLog(ERROR_LOG, BOT_L, "Weather API key not found");
    sendAsciiCatServiceUnavailable(request);
    return;
  }
  std::string keyStr(key);
  std::string city =
      request->commandArg.empty() ? DEFAULT_CITY : request->commandArg;
  std::string url = WEATHER_URL1;
  url += keyStr;
  url += WEATHER_URL2;
  url += city;
  url += QUOTE;

  Log::printLog(DEBUG_LOG, BOT_L, "URL: " + url);
  openCurl(request, url);
  logApiRequest(request->fdForApi, WEATHERAPI_HOST);
}

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
  for (stringVector::iterator it = _timeoutCat.begin(); it != itEnd; ++it) {
    std::ostringstream oss;
    oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
    sendMessageToServer(oss.str());
  }
}
