/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/17 15:12:35 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

  try {
    signal(SIGINT, Server::signalHandler);
    signal(SIGQUIT, Server::signalHandler);
    ser.runServer();
  } catch (const std::exception& e) {
    std::cerr << RED << e.what() << RESET << '\n';
    ser.closeServer();
    exit(EXIT_FAILURE);
  }

  ser.closeServer();
  std::cout << "The Server is closed" << std::endl;
  exit(EXIT_SUCCESS);
}
