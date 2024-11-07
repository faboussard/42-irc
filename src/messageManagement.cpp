/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messageManagement.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 09:15:40 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/07 15:33:17 by yusengok         ###   ########.fr       */
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

// To delete later --------------------------------------------------------->//
#ifdef DEBUG
static void clientIsAcceptedMessageToDelete(const Client *client,
                                            const std::string &command) {
  std::cout << BRIGHT_GREEN "CLIENT ACCEPTED !!!!!!!  WELCOME ^__^"
             << std::endl;
  std::cout << BLUE "NickName: " << client->getNickname() << std::endl;
  std::cout << "UserName: " << client->getUserName() << std::endl;
  std::cout << BRIGHT_YELLOW "Command: " << command << std::endl;
}
#endif
// <-------------------------------------------------------------------------//

void Server::handleInitialMessage(Client *client, const std::string &msg) {
  commandVectorPairs splittedPair = Parser::parseCommandIntoPairs(msg);
  size_t vecSize = splittedPair.size();

  for (size_t it = 0; it < vecSize; ++it) {
    std::string command = splittedPair[it].first;
    std::string argument = splittedPair[it].second;
    std::cout << MAGENTA "Command: " << command << std::endl;
    std::cout << "Message: " << argument << RESET << std::endl;

    if (command == "QUIT") {
      quit(argument, client, &_clients);
      return;
    }
    if (client->isAccepted()) {
#ifdef DEBUG
       clientIsAcceptedMessageToDelete(client, command);
#endif
      handleCommand(command, argument, client->getFd());
    } else if (command == "CAP" && client->isCapSend() == false &&
               client->isPasswordGiven() == false) {
      if (client->isCapSend() == false) client->setCapSend(true);
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
      send461NeedMoreParams(*client, "PASS");
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
        testAllNumericReplies(_startTime, client, "COMMAND", "puppy");
#endif
      }
    } else if (client->isAccepted() == false) {
      if (client->isNicknameSet() == false) send431NoNicknameGiven(*client);
      if (client->isUsernameSet() == false) send451NotRegistered(*client);
    }
  }
}

void Server::handleOtherMessage(const Client &client, const std::string &msg) {
  commandVectorPairs splittedPair = Parser::parseCommandIntoPairs(msg);
  size_t vecSize = splittedPair.size();
  for (size_t it = 0; it < vecSize; ++it) {
    std::string command = splittedPair[it].first;
    std::string argument = splittedPair[it].second;
    Command cmd = Parser::choseCommand(command);
    std::cout << BLUE "NickName: " << client.getNickname() << std::endl;
    std::cout << "UserName: " << client.getUserName() << std::endl;
    std::cout << BRIGHT_YELLOW "Command: " << command << std::endl;
    std::cout << MAGENTA "Message: " << argument << RESET << std::endl;
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
    std::cerr << RED "Error while receiving message" RESET << std::endl;
    return;
  } else if (valread == 0) {
    std::cout << "Client " << fd << " disconnected" << std::endl;
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
    std::cout << "Received message from client " << fd << ", nickname: "
              << _clients[fd].getNickname() << ": " << message
              << std::endl;

    Client &client = _clients[fd];
    if (client.isAccepted() == false) {
      handleInitialMessage(&client, message);
    } else {
      handleOtherMessage(client, message);
    }
  // }
}
/*============================================================================*/
/*       Commands management                                                  */
/*============================================================================*/

void Server::handleCommand(const std::string &command, std::string &argument,
                           int fd) {
  if (command.empty()) return;
  if (command == "JOIN") {
     joinChannel(argument, fd);
  } else if (command == "KICK") {
    // Exclure un client du canal
  } else if (command == "INVITE") {
    // Notice
  } else if (command == "TOPIC") {
    // Changer le sujet du canal
  } else if (command == "MODE") {
    // Changer le sujet du canal
  } else if (command == "WHO") {
    who(_clients.at(fd), argument);
  } else if (command == "LIST") {
    list(_clients.at(fd), argument);
  } else if (command == "NOTICE") {
    // Notice}
  } else if (command == "NICK") {
    Parser::verifyNick(argument, &_clients[fd], &_clients);
  } else if (command == "USER") {
    Parser::verifyUser(argument, &_clients[fd], &_clients);
  } else if (command == "PRIVMSG") {
    // Envoyer un message privé
  } else if (command == "QUIT") {
    quit(argument, &_clients[fd], &_clients);
  } else if (command == "PING") {
    ping(&_clients.at(fd), argument);
  } else if (command == "PASS" || command == "USER") {
    if (argument.empty())
      send461NeedMoreParams(_clients[fd], command);
    else
      send462AlreadyRegistered(_clients[fd]);
  } else {
    // Commande inconnue
  }
}
