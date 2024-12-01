/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainBot.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 09:09:16 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/01 16:36:37 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "../../includes/Bot.hpp"
#include "../../includes/Log.hpp"
#include "../../includes/Server.hpp"

static void exportEnv(const std::string& filename) {
  std::ifstream envFile(filename.c_str());
  if (!envFile.is_open()) {
    Log::printLog(ERROR_LOG, BOT_L, "Failed to open env file");
    return;
  }
  std::string line;
  while (std::getline(envFile, line)) {
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
      std::string key = line.substr(0, pos);
      std::string value = line.substr(pos + 1);
      setenv(key.c_str(), value.c_str(), 1);
      Log::printLog(DEBUG_LOG, BOT_L, "Exported env: " + key);
    }
  }
  envFile.close();
}

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
  exportEnv(ENV_FILE);
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
