/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   processCommands.cpp                                 :+:      :+:    :+: */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 10:31:27 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/27 10:44:58 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

void Bot::bot(BotRequest *request) {
  if (request->arg == "welcome")
    Log::printLog(INFO_LOG, BOT_L,
                  "Welcome message sent to " + request->clientNickname);
  // sendWelcomeMessage(request->clientNickname);
  else if (request->arg == "menu")
    Log::printLog(INFO_LOG, BOT_L,
                  "Bot menu sent to " + request->clientNickname);
  // sendInstructions(request->clientNickname);
}

void Bot::numbers(BotRequest *request) {
  _requestDatas.push_back(*request);
  // send API request to numbersapi.com
}

void Bot::joke(BotRequest *request) {
  _requestDatas.push_back(*request);
  // send API request to icanhazdadjoke.com
}

void Bot::film(BotRequest *request) {
  _requestDatas.push_back(*request);
  // send API request to omdbapi.com
}

void Bot::insultMe(BotRequest *request) {
  _requestDatas.push_back(*request);
  // send API request to evilinsult.com
}
