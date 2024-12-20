/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   messageManagement.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 09:15:40 by mbernard          #+#    #+#             */
/*   Updated: 2024/12/07 16:30:25 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <string>

#include "../includes/Log.hpp"
#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"
#include "../includes/colors.hpp"

static bool isLastPass(const stringPairsVector &splittedPair, size_t it,
                       size_t vecSize) {
  while (it < vecSize && splittedPair[it].first != "PASS") {
    ++it;
  }
  if (it >= vecSize) {
    return (true);
  }
  return (false);
}

static bool isLastNick(const stringPairsVector &splittedPair, size_t it,
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

void Server::handleInitialMessage(Client *client, const std::string &msg) {
  stringPairsVector splittedPair = Parser::parseCommandIntoPairs(msg, *client);
  size_t vecSize = splittedPair.size();

  for (size_t it = 0; it < vecSize; ++it) {
    Command command = Parser::chooseCommand(splittedPair[it].first);
    std::string argument = splittedPair[it].second;
    std::ostringstream oss;
    oss << "Command: " MAGENTA << commandToString(command)
        << RESET " | Message: " MAGENTA << argument << RESET;
    Log::printLog(DEBUG_LOG, PARSER, oss.str());

    if (command == QUIT) {
      quit(argument, client, &_clients);
      return;
    }
    if (client->isAccepted()) {
#ifdef DEBUG
      std::ostringstream oss;
      oss << "Nick:" << client->getNickname() << " has been accepted";
      Log::printLog(DEBUG_LOG, PARSER, oss.str());
#endif
      if (command == UNKNOWN)
        send421UnknownCommand(*client, splittedPair[it].first);
      else
        handleCommand(command, argument, client->getFd());
    } else if (command == CAP) {
      continue;
    } else if (command == PASS) {
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
    } else if (command == NICK) {
      if (isLastNick(splittedPair, it + 1, vecSize)) {
        if (Parser::verifyNick(argument, client, &_clients, _channels, false) ==
            true && client->isAccepted() == false && client->isUsernameSet()) {
          client->declareAccepted();
          sendConnectionMessage(*client);
        }
      }
    } else if (command == USER) {
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

void Server::handleOtherMessage(const Client &client, const std::string &msg) {
#ifdef DEBUG
  std::ostringstream oss;
  oss << "Nick: " BLUE << client.getNickname() << RESET << " | UName: " BLUE
      << client.getUserName() << RESET << " | Message: " MAGENTA << msg << RESET
      << " | Message size: " << msg.size();
  Log::printLog(DEBUG_LOG, PARSER, oss.str());
#endif
  stringPairsVector splittedPair = Parser::parseCommandIntoPairs(msg, client);
  size_t vecSize = splittedPair.size();
  int fd = client.getFd();
  for (size_t it = 0; it < vecSize; ++it) {
    std::string command = splittedPair[it].first;
    std::string argument = splittedPair[it].second;
    Command cmd = Parser::chooseCommand(command);
    clientsMap::iterator itCli = _clients.find(fd);
    if (itCli == _clients.end()) return;
#ifdef DEBUG
    {
      std::ostringstream oss;
      oss << "Nick:" BLUE << client.getNickname() << RESET << " | UName:" BLUE
          << client.getUserName() << RESET << " | Command:" BRIGHT_YELLOW
          << command << RESET << " | Message:" MAGENTA << argument << RESET;
      Log::printLog(DEBUG_LOG, PARSER, oss.str());
    }
#endif
    if (cmd == UNKNOWN) {
      send421UnknownCommand(client, command);
      continue;
    }
    if (cmd == CAP) continue;
    handleCommand(cmd, argument, client.getFd());
  }
}

/*============================================================================*/
/*       Clients management                                                   */
/*============================================================================*/

bool Server::isMessageEmpty(std::string *message) {
  message->erase(0, message->find_first_not_of("\n\r"));
  message->erase(message->find_last_not_of("\n\r") + 1);
#ifdef DEBUG
  std::ostringstream oss;
  oss << "Message: " << *message;
  Log::printLog(DEBUG_LOG, PARSER, oss.str());
#endif
  return message->empty();
}

void Server::handleClientMessage(int fd) {
  static std::map<int, std::string> messageBuffer;
  char buffer[1024] = {0};
  std::memset(buffer, 0, sizeof(buffer));
  int valread = recv(fd, buffer, sizeof(buffer), 0);

  if (valread == -1) {
    std::ostringstream oss;
    oss << "fd" << fd << ": Error occurred while receiving a message.";
    Log::printLog(ERROR_LOG, CLIENT, oss.str());
    return;
  } else if (valread == 0) {
    std::ostringstream oss;
    oss << "fd" << fd << ": Connection closed by peer";
    Log::printLog(NOTIFY_LOG, CLIENT, oss.str());
    messageBuffer[fd].erase();
    clearClient(fd);
    return;
  }
  messageBuffer[fd] += std::string(buffer, valread);
  size_t pos;
  std::string message = "";
  while ((pos = messageBuffer[fd].find("\r\n")) != std::string::npos) {
    message += messageBuffer[fd].substr(0, pos + 2);
    messageBuffer[fd].erase(0, pos + 1);
  }
  if (isMessageEmpty(&message)) return;
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

void Server::handleCommand(Command command, const std::string &argument,
                           int fd) {
  switch (command) {
    case JOIN:
      joinChannel(fd, argument);
      break;
    case PART:
      part(fd, argument);
      break;
    case KICK:
      kick(fd, argument);
      break;
    case INVITE:
      invite(fd, argument);
      break;
    case TOPIC:
      topic(fd, argument);
      break;
    case MODE:
      mode(fd, argument);
      break;
    case WHO:
      who(_clients.at(fd), argument);
      break;
    case LIST:
      list(_clients.at(fd), argument);
      break;
    case NICK:
      Parser::verifyNick(argument, &_clients[fd], &_clients, _channels, true);
      break;
    case USER:
      Parser::verifyUser(argument, &_clients[fd], &_clients);
      break;
    case PRIVMSG:
      privmsg(fd, argument);
      break;
    case QUIT:
      quit(argument, &_clients[fd], &_clients);
      break;
    case PING:
      ping(_clients.at(fd), argument);
      break;
    case PASS:
      send462AlreadyRegistered(_clients[fd]);
      break;
    default:
      break;
  }
}
