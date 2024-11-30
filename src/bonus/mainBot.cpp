/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainBot.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 09:09:16 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/30 20:36:45 by yusengok         ###   ########.fr       */
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
                         const std::string& password, int* serverPort) {
  errno = 0;
  int64_t serverPortNum = std::strtol(serverPortStr.c_str(), NULL, 10);
  if (errno == ERANGE || serverPortNum <= 1080 || serverPortNum > 65535) {
    std::cerr << "Server port number is out of range: it sohuld be between "
                 "1081 and 65535)"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  if (password.length() > 1000) {
    std::cerr << "Invalid password format" << std::endl;
    exit(EXIT_FAILURE);
  }
  *serverPort = static_cast<int>(serverPortNum);
}

int main(int ac, char** argv) {
  if (ac != 3) {
    std::cerr << "Usage: ./ircbot <server port> <server pass>"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  int serverPort;
  checkBotArgs(argv[1], argv[2], &serverPort);
  Bot ircbot(serverPort, argv[2]);

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
