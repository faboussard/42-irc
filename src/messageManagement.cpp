/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messageManagement.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/10/31 15:05:59 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"
#include "../includes/colors.hpp"

static bool isLastPass(const commandVectorPairs &splittedPair, size_t it,
                       size_t vecSize) {
  while (it < vecSize && splittedPair[it].first != "PASS") {
    ++it;
  }
  if (it >= vecSize) {
    return (true);
  }
  return (false);
}

static bool isLastNick(const commandVectorPairs &splittedPair, size_t it,
                       size_t vecSize) {
  while (it < vecSize && splittedPair[it].first != "NICK" &&
         splittedPair[it].first != "USER") {
    ++it;
  }
  if (it >= vecSize || splittedPair[it].first == "USER") {
    return (true);
  }
  return (false);
}

void Server::handleInitialMessage(Client &client, const std::string &message) {
  commandVectorPairs splittedPair = Parser::parseCommandIntoPairs(message);
  size_t vecSize = splittedPair.size();

  for (size_t it = 0; it < vecSize; ++it) {
    std::string command = splittedPair[it].first;
    std::string argument = splittedPair[it].second;
    std::cout << MAGENTA "Command: " << command << std::endl;
    std::cout << "Message: " << argument << RESET << std::endl;

    if (command == "QUIT") {
      // lancer la commande QUIT avec les arguments : quit(client, argument);
      clearClient(client.getFd());
      return;
    }
    if (client.isAccepted()) {
      std::cout << BRIGHT_GREEN "CLIENT ACCEPTED !!!!!!!  WELCOME ^__^"
                << std::endl;
      std::cout << BLUE "NickName: " << client.getNickName() << std::endl;
      std::cout << "UserName: " << client.getUserName() << std::endl;
      std::cout << BRIGHT_YELLOW "Command: " << command << std::endl;
      handleCommand(command, argument, client.getFd());
    } else if (command == "CAP" && client.isCapSend() == false &&
        client.isPasswordGiven() == false) {
      if (client.isCapSend() == false) client.setCapSend(true);
    } else if (command == "PASS") {
      if (isLastPass(splittedPair, it + 1, vecSize)) {
        if (Parser::verifyPassword(argument, _password, client) == false) {
          if (client.getNbPassAttempts() >= 3) {
            clearClient(client.getFd());
            return;
          }
        }
      }
    } else if (client.isPasswordGiven() == false) {
      send461NeedMoreParams(client.getFd(), "", "PASS");
    } else if (command == "NICK") {
      if (isLastNick(splittedPair, it + 1, vecSize))
        Parser::verifyNick(argument, client, _clients);
    } else if (command == "USER") {
      Parser::verifyUser(argument, client, _clients);
    } else if (client.isAccepted() == false) {
      if (client.isPasswordGiven() && client.isNicknameSet() &&
          client.isUsernameSet()) {
        client.declareAccepted();
        sendConnectionMessage(client);
        handleCommand(command, argument, client.getFd());
      } else {
        if (client.isNicknameSet() == false)
          send431NoNicknameGiven(client.getFd(), "");
        if (client.isUsernameSet() == false)
          send461NeedMoreParams(client.getFd(), client.getNickName(), "USER");
      }
    }
  }
}

void Server::handleOtherMessage(Client &client, const std::string &message) {
  commandVectorPairs splittedPair = Parser::parseCommandIntoPairs(message);
  size_t vecSize = splittedPair.size();
  for (size_t it = 0; it < vecSize; ++it) {
    std::string command = splittedPair[it].first;
    std::string argument = splittedPair[it].second;
    Command cmd = Parser::choseCommand(command);
    std::cout << BLUE "NickName: " << client.getNickName() << std::endl;
    std::cout << "UserName: " << client.getUserName() << std::endl;
    std::cout << BRIGHT_YELLOW "Command: " << command << std::endl;
    std::cout << MAGENTA "Message: " << argument << RESET << std::endl;
    if (cmd == UNKNOWN) {
      send421UnknownCommand(client.getFd(), client.getNickName(), command);
      continue;
    }
    if (cmd == CAP) continue;
    handleCommand(command, argument, client.getFd());
  }
}

/* Clients Management */

void Server::handleClientMessage(int fd) {
  char buffer[1024] = {0};
  std::memset(buffer, 0, sizeof(buffer));
  int valread = recv(fd, buffer, sizeof(buffer), 0);

  switch (valread) {
    case -1:
      std::cerr << RED "Error while receiving message" RESET << std::endl;
      // fallthrough
    case 0:
      std::cout << "Client " << fd << " disconnected" << std::endl;
      clearClient(fd);
      return;
  }
  std::string message(buffer, valread);
  std::cout << "Received message from client " << fd << ": " << message
            << std::endl;

  Client &client = _clients[fd];
  if (client.isAccepted() == false) {
    handleInitialMessage(client, message);
  } else {
    // sendToAllClients(message);
    handleOtherMessage(client, message);
  }
}

/*  Commands management */

void Server::handleCommand(const std::string &command, std::string &argument,
                           int fd) {
  if (command.empty()) return;
  if (command == "JOIN") {
    // joinChannel(argument, fd);
  } else if (command == "KICK") {
    // Exclure un client du canal
  } else if (command == "INVITE") {
    // Notice
  } else if (command == "TOPIC") {
    // Changer le sujet du canal
  } else if (command == "MODE") {
    // Changer le sujet du canal
  } else if (command == "LIST") {
    // Lister les canaux
  } else if (command == "NOTICE") {
    // Notice}
  } else if (command == "NICK") {
    Parser::verifyNick(argument, _clients[fd], _clients);
  } else if (command == "USER") {
    Parser::verifyUser(argument, _clients[fd], _clients);
  } else if (command == "PRIVMSG") {
    // Envoyer un message privé
  } else if (command == "QUIT") {
    // Déconnecter le client
  } else if (command == "PING") {
    // client.sendNumericReply(1, "PONG");
  } else if (command == "PASS" || command == "USER") {
    if (argument.empty())
      send461NeedMoreParams(fd, "", command);
    else
      send462AlreadyRegistered(fd, _clients[fd].getNickName());
  } else {
    // Commande inconnue
  }
}
