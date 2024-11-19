/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/19 15:25:45 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

#include "../includes/Bot.hpp"
#include "../includes/Config.hpp"
#include "../includes/Server.hpp"
#include "../includes/colors.hpp"

void checkArgs(int port, const std::string& password) {
  if (port <= 1080 || port > 65535) {
    std::cerr << "Invalid port number (ports accepted : 1081 to 65535)"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  if (password.length() > 1000) {
    std::cerr << "Password too long" << std::endl;
    exit(EXIT_FAILURE);
  }
}

int main(int ac, char** argv) {
  if (ac != 3) {
    std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
    exit(EXIT_FAILURE);
  }

  int port = std::atoi(argv[1]);
  std::string password = argv[2];
  checkArgs(port, password);
  Server ser(port, password);
  Bot bot(port);

  try {
    signal(SIGINT, Server::signalHandler);
    signal(SIGQUIT, Server::signalHandler);
    initServerConfig();
    ser.runServer();
    bot.runBot();
  } catch (const std::runtime_error& e) {
    std::cerr << RED << e.what() << RESET << '\n';
    ser.closeServer();
    return (EXIT_FAILURE);
  }
  ser.closeServer();
  Server::printLog(INFO_LOG, SYSTEM, "Server shut down");
  return (EXIT_SUCCESS);
}
