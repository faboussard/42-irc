/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/17 11:53:10 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/colors.hpp"

void checkArgs(int port, const std::string& password) {
  if (port < 1080 || port > 65535) {
    std::cerr << RED "Invalid port number" RESET << std::endl;
    exit(EXIT_FAILURE);
  }
  if (password.length() > 1000) {
    std::cerr << RED "Password too long" RESET << std::endl;
    exit(EXIT_FAILURE);
  }
}

int main(int ac, char** argv) {
  std::string usage = "Usage: ./server <port> <password>";
  if (ac != 3) {
    std::cerr << usage << std::endl;
    exit(EXIT_FAILURE);
  }

  int port = std::atoi(argv[1]);
  std::string password = argv[2];
  checkArgs(port, password);
  Server ser(port);

  try {
    signal(SIGINT, Server::signalHandler);
    signal(SIGQUIT, Server::signalHandler);
    ser.runServer();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    ser.closeServer();
  }

    std::cout << "The Server is closed" << std::endl;
    exit(EXIT_SUCCESS);
}
