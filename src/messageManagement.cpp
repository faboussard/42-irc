/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messageManagement.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 09:15:40 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/18 11:27:54 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <string>

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

// To delete later --------------------------------------------------------->//
#ifdef DEBUG
static void clientIsAcceptedMessageToDelete(const Client *client,
                                            const std::string &command) {
  std::cout << BRIGHT_GREEN "CLIENT ACCEPTED !!!!!!!  WELCOME ^__^"
            << std::endl;
  std::cout << BLUE "NickName: " << client->getNickname() << std::endl;
  std::cout << "UserName: " << client->getUserName() << std::endl;
  std::cout << BBRIGHT_YELLOW "Command: " << command << std::endl;
}
#endif
// <-------------------------------------------------------------------------//

void Server::handleInitialMessage(Client *client, const std::string &msg) {
  commandVectorPairs splittedPair = Parser::parseCommandIntoPairs(msg);
  size_t vecSize = splittedPair.size();

  for (size_t it = 0; it < vecSize; ++it) {
    std::string command = splittedPair[it].first;
    std::string argument = splittedPair[it].second;
    // std::cout << MAGENTA "Command: " << command << std::endl;
    // std::cout << "Message: " << argument << RESET << std::endl;
    std::ostringstream oss;
    oss << "Command: " MAGENTA << command << RESET " | Message: " MAGENTA
        << argument << RESET;
    printLog(DEBUG_LOG, PARSER, oss.str());

    if (command == "QUIT") {
      quit(argument, client, &_clients);
      return;
    }
    if (client->isAccepted()) {
#ifdef DEBUG
      clientIsAcceptedMessageToDelete(client, command);
#endif
      handleCommand(command, argument, client->getFd());
    } else if (command == "CAP") {
      continue;
    } else if (command == "PASS") {
      if (isLastPass(splittedPair, it + 1, vecSize)) {
        if (Parser::verifyPassword(argument, _password, client) == false) {
          if (client->getNbPassAttempts() >= 3) {
            clearClient(client->getFd());
            return;
          }
        }
      }
    } else if (client->isPasswordGiven() == false) {
      send451NotRegistered(*client);
    } else if (command == "NICK") {
      if (isLastNick(splittedPair, it + 1, vecSize)) {
        if (Parser::verifyNick(argument, client, &_clients) == true &&
            client->isAccepted() == false && client->isUsernameSet()) {
          client->declareAccepted();
          sendConnectionMessage(*client);
        }
      }
    } else if (command == "USER") {
      if (Parser::verifyUser(argument, client, &_clients) == true &&
          client->isAccepted() == false && client->isNicknameSet()) {
        client->declareAccepted();
        sendConnectionMessage(*client);
#ifdef TESTNUMERICR
        testAllNumericReplies(_startTime, *client, "COMMAND", "puppy");
#endif
      }
    } else if (client->isAccepted() == false) {
      send451NotRegistered(*client);
    }
  }
}

#ifdef DEBUG
static void clientNameUserCommandMessage(
    const Client *client, const std::string *command,
    const std::string *argument, const int *fd) {

  std::cout << BLUE "NickName: " << client->getNickname() << std::endl;
  std::cout << "UserName: " << client->getUserName() << std::endl;
  std::cout << BBRIGHT_YELLOW "Command: " << *command << RESET << std::endl;
  std::cout << MAGENTA "Message: " << *argument << RESET << std::endl;
  (void)fd;
}
#endif

void Server::handleOtherMessage(const Client &client, const std::string &msg) {
  commandVectorPairs splittedPair = Parser::parseCommandIntoPairs(msg);
  size_t vecSize = splittedPair.size();
  int fd = client.getFd();
  for (size_t it = 0; it < vecSize; ++it) {
    std::string command = splittedPair[it].first;
    std::string argument = splittedPair[it].second;
    Command cmd = Parser::choseCommand(command);
    clientsMap::iterator itCli = _clients.find(fd);
    if (itCli == _clients.end())
      return;
#ifdef DEBUG
    clientNameUserCommandMessage(&client, &command, &argument, &fd);
#endif
    if (cmd == UNKNOWN) {
      send421UnknownCommand(client, command);
      continue;
    }
    if (cmd == CAP) continue;
    handleCommand(command, argument, client.getFd());
  }
}

/*============================================================================*/
/*       Clients management                                                   */
/*============================================================================*/

void Server::handleClientMessage(int fd) {
  static std::map<int, std::string> messageBuffer;
  char buffer[1024] = {0};
  std::memset(buffer, 0, sizeof(buffer));
  int valread = recv(fd, buffer, sizeof(buffer), 0);

  if (valread == -1) {
    // std::cerr << RED "Error while receiving message" RESET << std::endl;
    std::ostringstream oss;
    oss << "fd" << fd << ": Error occurred while receiving a message.";
    Server::printLog(ERROR_LOG, CLIENT, oss.str());
    return;
  } else if (valread == 0) {
    // std::cout << "Client " << fd << " disconnected" << std::endl;
    messageBuffer[fd].erase();
    clearClient(fd);
    return;
  }

  // Accumuler les données reçues
  messageBuffer[fd] += std::string(buffer, valread);

  // Vérifier si le message est complet (par exemple, contient une nouvelle
  // ligne)
  size_t pos;
  std::string message = "";
  while ((pos = messageBuffer[fd].find("\r\n")) != std::string::npos) {
    message += messageBuffer[fd].substr(0, pos + 2);
    messageBuffer[fd].erase(0, pos + 1);
  }
  if (message.empty())
    return;
  // std::cout << "Received message from client " << fd
  //           << ", nickname: " << _clients[fd].getNickname() << ": " << message
  //           << std::endl;
  std::string msgBuf = message;
  msgBuf.erase(0, msgBuf.find_first_not_of("\n"));
  msgBuf.erase(msgBuf.find_last_not_of("\n") + 1);
  std::ostringstream oss;
  oss << _clients[fd].getNickname() << " (fd" << fd << ") sent a message: "
      << msgBuf;
  printLog(INFO_LOG, CLIENT, oss.str());

  Client &client = _clients[fd];
  if (client.isAccepted() == false) {
    handleInitialMessage(&client, message);
  } else {
    handleOtherMessage(client, message);
  }
}

/*============================================================================*/
/*       Commands management                                                  */
/*============================================================================*/

void Server::handleCommand(const std::string &command,
                           const std::string &argument, int fd) {
  if (command.empty()) return;
  if (command == "JOIN") {
    joinChannel(argument, fd);
  } else if (command == "KICK") {
    kick(fd, argument);
  } else if (command == "INVITE") {
    invite(fd, argument);
  } else if (command == "TOPIC") {
    topic(fd, argument);
  } else if (command == "MODE") {
    mode(fd, argument);
  } else if (command == "WHO") {
    who(_clients.at(fd), argument);
  } else if (command == "LIST") {
    list(_clients.at(fd), argument);
  } else if (command == "NICK") {
    Parser::verifyNick(argument, &_clients[fd], &_clients);
  } else if (command == "USER") {
    Parser::verifyUser(argument, &_clients[fd], &_clients);
  } else if (command == "PRIVMSG") {
    privmsg(fd, argument);
  } else if (command == "QUIT") {
    quit(argument, &_clients[fd], &_clients);
  } else if (command == "PING") {
    ping(_clients.at(fd), argument);
  } else if (command == "PASS" || command == "USER") {
    if (argument.empty())
      send461NeedMoreParams(_clients[fd], command);
    else
      send462AlreadyRegistered(_clients[fd]);
  } else {
    // Commande inconnue
  }
}
