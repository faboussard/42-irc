/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainBot.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 09:09:16 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/27 12:44:25 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <string>

#include "../../includes/Bot.hpp"
#include "../../includes/Log.hpp"
#include "../../includes/Server.hpp"

static void checkBotArgs(const std::string& serverPortStr,
                         const std::string& password,
                         const std::string& botPortStr, int* serverPort,
                         int* botPort) {
  errno = 0;
  int64_t serverPortNum = std::strtol(serverPortStr.c_str(), NULL, 10);
  if (errno == ERANGE || serverPortNum <= 1080 || serverPortNum > 65535) {
    std::cerr << "Server port number is out of range: it sohuld be between "
                 "1081 and 65535)"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  int64_t botPortNum = std::strtol(botPortStr.c_str(), NULL, 10);
  errno = 0;
  if (errno == ERANGE || botPortNum <= 1080 || botPortNum > 65535) {
    std::cerr << "Bot port number is out of range: it sohuld be between "
                 "1081 and 65535)"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  if (botPortNum == serverPortNum) {
    std::cerr << "Bot port number should be different from Server port number"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  if (password.length() > 1000) {
    std::cerr << "Invalid password format" << std::endl;
    exit(EXIT_FAILURE);
  }
  *serverPort = static_cast<int>(serverPortNum);
  *botPort = static_cast<int>(botPortNum);
}

int main(int ac, char** argv) {
  if (ac != 4) {
    std::cerr << "Usage: ./ircbot <server port> <server pass> <bot port>"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  int serverPort;
  int botPort;
  checkBotArgs(argv[1], argv[2], argv[3], &serverPort, &botPort);
  Bot ircbot(serverPort, argv[2], botPort);

  try {
    signal(SIGINT, Bot::signalHandler);
    signal(SIGQUIT, Bot::signalHandler);
    ircbot.runBot();
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return (EXIT_FAILURE);
  }
  Log::printLog(INFO_LOG, BOT_L, "Bot shut down");
  return (EXIT_SUCCESS);
}
