/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processCommands.cpp                                 :+:      :+:    :+: */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 10:31:27 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/28 13:08:36 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../../includes/Bot.hpp"

void Bot::displayAsciiByCommand(BotRequest *request, eBotCommand command) {
  if (command == MENU) {
    Log::printLog(INFO_LOG, BOT_L,
                  "Bot menu sent to " + request->clientNickname);

    stringVector::const_iterator itEnd = _instructions.end();
    for (std::vector<std::string>::const_iterator it = _instructions.begin();
         it != itEnd; ++it) {
      std::ostringstream oss;
      oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
      sendMessageToServer(oss.str());
    }
  } 
  else if (command == INSULTME) {
    stringVector insults;
    insults.push_back(INSULTME_CAT_1);
    insults.push_back(INSULTME_CAT_2);
    insults.push_back(INSULTME_CAT_3);
    insults.push_back(INSULTME_CAT_4);

    for (std::vector<std::string>::const_iterator it = insults.begin();
         it != insults.end(); ++it) {
      std::ostringstream oss;
      oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
      sendMessageToServer(oss.str());
    }
  } 
  else if (command == JOKE) {
    stringVector joke;
    joke.push_back(JOKE_CAT_1);
    joke.push_back(JOKE_CAT_2);
    joke.push_back(JOKE_CAT_3);

    for (std::vector<std::string>::const_iterator it = joke.begin();
         it != joke.end(); ++it) {
      std::ostringstream oss;
      oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
      sendMessageToServer(oss.str());
    }
  } 
  else if (command == ADVICE) {
    stringVector advice;
    advice.push_back(ADVICE_CAT_1);
    advice.push_back(ADVICE_CAT_2);
    advice.push_back(ADVICE_CAT_3);
    advice.push_back(ADVICE_CAT_4);

    for (std::vector<std::string>::const_iterator it = advice.begin();
         it != advice.end(); ++it) {
      std::ostringstream oss;
      oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
      sendMessageToServer(oss.str());
    }
  } 
  else {  // UNKNOWN / DEFAULT
    stringVector unknown;
    unknown.push_back(DEFAULT_CAT_1);
    unknown.push_back(DEFAULT_CAT_2);
    unknown.push_back(DEFAULT_CAT_3);

    for (std::vector<std::string>::const_iterator it = unknown.begin();
         it != unknown.end(); ++it) {
      std::ostringstream oss;
      oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
      sendMessageToServer(oss.str());
    }
  }
}


void Bot::joke(BotRequest *request) {
  std::string curlCommand = "curl -s ";
  curlCommand += JOKE_URL;
  FILE *fp = popen(curlCommand.c_str(), "r");
  if (fp == NULL) {
    // Handle error
  }
  request->fpForApi = fp;
  request->fdForApi = fileno(fp);
  _requestDatas.push_back(*request);

  struct pollfd newPoll;
  newPoll.fd = request->fdForApi;
  newPoll.events = POLLIN;
  newPoll.revents = 0;
  _botPollFds.push_back(newPoll);

  // Set timeout for API response

  logApiRequest(request->fdForApi, JOKEAPI_HOST);
}

void Bot::insultMe(BotRequest *request) {
  std::string curlCommand = "curl -s ";  // -s for silent mode
  curlCommand += INSULTME_URL;
  FILE *fp = popen(curlCommand.c_str(), "r");
  if (fp == NULL) {
    // Handle error
  }
  request->fpForApi = fp;
  request->fdForApi = fileno(fp);
  _requestDatas.push_back(*request);

  struct pollfd newPoll;
  newPoll.fd = request->fdForApi;
  newPoll.events = POLLIN;
  newPoll.revents = 0;
  _botPollFds.push_back(newPoll);

  // Set timeout for API response

  logApiRequest(request->fdForApi, INSULTMEAPI_HOST);
}

void Bot::advice(BotRequest *request) {
  _requestDatas.push_back(*request);
  // send API request to omdbapi.com
}
