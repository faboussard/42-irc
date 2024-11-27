/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processCommands.cpp                                 :+:      :+:    :+: */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 10:31:27 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/27 14:36:40 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../../includes/Bot.hpp"

void Bot::bot(BotRequest *request) {
  if (request->arg == "welcome") {
    Log::printLog(INFO_LOG, BOT_L,
                  "Welcome message sent to " + request->clientNickname);

    stringVector::const_iterator itEnd = _instructions.end();
    for (std::vector<std::string>::const_iterator it = _instructions.begin();
         it != itEnd; ++it) {
      std::ostringstream oss;
      oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
      sendMessageToServer(oss.str());
    } 
  } else if (request->arg == "menu") {
    Log::printLog(INFO_LOG, BOT_L,
                  "Bot menu sent to " + request->clientNickname);
  // sendInstructions(request->clientNickname);
  }
}

void Bot::numbers(BotRequest *request) {
  _requestDatas.push_back(*request);
  // send API request to numbersapi.com
}

void Bot::joke(BotRequest *request) {
  _requestDatas.push_back(*request);
  std::string curlCommand = "curl -s ";  // -s for silent mode
  curlCommand += JOKE_URL;
  FILE *fp = popen(curlCommand.c_str(), "r");
  // if (fp == NULL) error

  request->fpForApi = fp;
  request->fdForApi = fileno(fp);
#ifdef DEBUG
  std::ostringstream oss;
  oss << "fd" << request->fdForApi << ": API request sent to " << JOKE_URL;
  Log::printLog(DEBUG_LOG, BOT_L, oss.str());
#endif
}

void Bot::film(BotRequest *request) {
  _requestDatas.push_back(*request);
  // send API request to omdbapi.com
}

void Bot::insultMe(BotRequest *request) {
  _requestDatas.push_back(*request);
  // send API request to evilinsult.com
}
