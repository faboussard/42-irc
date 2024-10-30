/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/30 10:57:08 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

#include "../includes/Parser.hpp"
#include "../includes/colors.hpp"
#include "../includes/serverConfig.hpp"
#include "../includes/utils.hpp"

bool Server::_signal = false;

Server::Server(int port, std::string password) {
  _port = port;
  _password = password;
  _signal = false;
  _socketFd = -1;
  // _name = "coco";
}

/* Getters */
const Client &Server::getClientByFd(int fd) const {
  clientsMap::const_iterator it = _clients.find(fd);
  if (it == _clients.end()) {
    std::cerr << "Client not found with the given file descriptor" << std::endl;
    throw std::runtime_error("Client not found");
  }
  return it->second;
}

Channel &Server::getChannelByName(const std::string &name) {
  channelsMap::iterator it = _channels.find(name);
  if (it == _channels.end()) {
    std::cerr << "Channel not found with the given name" << std::endl;
    throw std::runtime_error("Channel not found");
  }
  return it->second;
}

// const std::string &Server::getServerName() const { return _name; }

const std::string &Server::getPassword() const { return _password; }

int Server::getPort() const { return _port; }

int Server::getSocketFd() const { return _socketFd; }


/* Server Mounting */

void Server::runServer() {
  createSocket();
  _startTime = fetchStartTime();
  std::cout << GREEN "Server started on port " RESET << _port << std::endl;
  monitorConnections();
}

void Server::createSocket() {
  _address.sin_family = AF_INET;
  _address.sin_addr.s_addr = INADDR_ANY;
  _address.sin_port = htons(_port);

  _socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_socketFd == -1) {
    throw std::runtime_error("Failed to create socket");
  }

  int en = 1;
  if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) {
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket");
  }

  if (fcntl(_socketFd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket");
  }

  if (bind(_socketFd, (struct sockaddr *)&_address, sizeof(_address)) == -1) {
    throw std::runtime_error(
        "Failed to bind socket. Port might be used elsewhere");
  }

  if (listen(_socketFd, SOMAXCONN) == -1) {
    throw std::runtime_error("Failed to listen on socket");
  }
}

std::string Server::fetchStartTime(void) {
  time_t now = time(0);
  std::string startTime = ctime(&now);
  startTime.erase(_startTime.find_last_not_of("\n") + 1);
  return (startTime);
}

void Server::monitorConnections() {
  struct pollfd newPoll;
  newPoll.fd = _socketFd;
  newPoll.events = POLLIN;
  newPoll.revents = 0;
  _pollFds.push_back(newPoll);
  while (_signal == false) {
    int pollResult = poll(&_pollFds[0], _pollFds.size(), -1);
    if (pollResult == -1 && _signal == false) {
      std::cerr << RED "Error while polling" RESET << std::endl;
      break;
    }
    for (size_t i = 0; i < _pollFds.size(); ++i) {
      if (_pollFds[i].revents & POLLIN && _signal == false) {
        if (_pollFds[i].fd == _socketFd) {
          acceptNewClient();
        } else {
          handleClientMessage(_pollFds[i].fd);
        }
      }
    }
  }
}

bool isLastPass(const commandVectorPairs &splittedPair, size_t it,
                size_t vecSize) {
  while (it < vecSize && splittedPair[it].first != "PASS") {
    ++it;
  }
  if (it >= vecSize) {
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

    if (command == "CAP" && client.isCapSend() == false && client.isPasswordGiven() == false) {
      client.setCapSend(true);
      continue;
    }
    if (command == "PASS") {
      if (isLastPass(splittedPair, it + 1, vecSize) &&
          Parser::verifyPassword(argument, _password, client) == false) {
        clearClient(client.getFd());
        return;
      }
    } else if (client.isPasswordGiven() == false) {
      std::cerr << RED "NO PASSWORD GIVEN !" RESET << std::endl;
      send461NeedMoreParams(client.getFd(), "", "PASS");
      clearClient(client.getFd());
      return;
    } else if (command == "NICK") {
      if (Parser::verifyNick(argument, client, _clients) == false) {
        clearClient(client.getFd());
        return;
      }
    } else if (command == "USER" && client.isNicknameSet()) {
      if (Parser::verifyUser(argument, client, _clients) == false) {
        std::cout << RED "Invalid username" RESET << std::endl;
        clearClient(client.getFd());
        return;
      }
    } else if (client.isNicknameSet() == false) {
      send431NoNicknameGiven(client.getFd(), "");
      clearClient(client.getFd());
      return;
    } else if (client.isUsernameSet() == false) {
      send461NeedMoreParams(client.getFd(), client.getNickName(), "USER");
      clearClient(client.getFd());
      return;
    } else if (client.isAccepted()) {
      if (command == "QUIT") {
        // lancer la commande QUIT avec les arguments : quit(client, argument);
        clearClient(client.getFd());
        return;
      } else {
        std::cout << CYAN << "OTHER COMMAND ! \ncommand = " << command
                  << "\nargument = " << argument << RESET << std::endl;
        handleCommand(command, argument, client.getFd());
      }
    }
    if (client.isPasswordGiven() && client.isNicknameSet() &&
        client.isUsernameSet())
      client.declareAccepted();
  }
}

void Server::handleOtherMessage(Client &client, const std::string &message) {
  commandVectorPairs splittedPair = Parser::parseCommandIntoPairs(message);
  size_t vecSize = splittedPair.size();
  for (size_t it = 0; it < vecSize; ++it) {
    std::string command = splittedPair[it].first;
    std::string argument = splittedPair[it].second;
    Command cmd = Parser::choseCommand(command);
    std::cout << MAGENTA "Command: " << command << std::endl;
    std::cout << "Message: " << argument << RESET << std::endl;
    if (cmd == UNKNOWN) {
      // ERR_UNKNOWNCOMMAND (421)
      std::cerr << RED "Unknown command" RESET << std::endl;
      continue;
    } else if (cmd == CAP) {
      continue;
    } else {
      handleCommand(command, argument, client.getFd());
    }
  }
}

void Server::closeServer() {
  // Fermer tous les clients
  for (clientsMap::iterator it = _clients.begin(); it != _clients.end(); it++) {
    closeClient(it->second.getFd());
  }
  _clients.clear();

  // Fermer le socket principal
  if (_socketFd != -1) {
    std::cout << RED "Server <" RESET << _socketFd << RED "> Disconnected" RESET
              << std::endl;
    close(_socketFd);
    _socketFd = -1;
  }
  _pollFds.clear();
  shrink_to_fit(_pollFds);
}

void Server::signalHandler(int signal) {
  if (signal == SIGINT || signal == SIGQUIT) {
    _signal = true;
    std::cout << std::endl << "Signal Received!" << std::endl;
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

void Server::acceptNewClient() {
  Client cli;
  struct sockaddr_in cliadd;
  struct pollfd newPoll;
  socklen_t len = sizeof(cliadd);

  int newClientFd = accept(_socketFd,
                          reinterpret_cast<sockaddr *>(&cliadd), &len);
  if (newClientFd == -1) {
    std::cerr << RED "Failed to accept new client" RESET << std::endl;
    return;
  }

  if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) == -1) {
    std::cerr << "fcntl() failed" << std::endl;
    return;
  }

  newPoll.fd = newClientFd;
  newPoll.events = POLLIN;
  newPoll.revents = 0;

  cli.setFd(newClientFd);
  cli.setIp(inet_ntoa(cliadd.sin_addr));  // inet_ntoa = convertit l'adresse
                                          // IP en une chaîne de caractères

  // ----- For test ---------
  cli.setUInvisibleMode(true);
  cli.setUOperatorMode(false);
  cli.setURegisteredMode(true);
  testAllNumericReplies(_startTime, cli, "COMMAND", "puppy");
  // ------------------------

  _clients[newClientFd] = cli;
  _pollFds.push_back(newPoll);

  std::cout << GREEN "New client connected: " RESET << newClientFd << std::endl;
  sendConnectionMessage(cli);
}

void Server::sendConnectionMessage(const Client &target) const {
  std::string nick = target.getNickName().empty() ? "*" : target.getNickName();
  std::string user = target.getUserName().empty() ? "*" : target.getUserName();
  std::string host = target.getIp().empty() ? "*" : target.getIp();
  int fd = target.getFd();
  sendWelcome(fd, nick);
  send001Welcome(fd, nick, user, host);
  send002Yourhost(fd, nick);
  send003Created(fd, nick, _startTime);
  send104Myinfo(fd, nick);
  send005Isupport(fd, nick, TOKENS);
}

void Server::sendToAllClients(const std::string &message) {
  for (clientsMap::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    if (it->second.isAccepted()) it->second.receiveMessage(message);
  }
}

void Server::closeClient(int fd) {
  if (fd != -1) {
    close(fd);
    std::cout << RED "Client <" RESET << fd << RED "> Disconnected" RESET
              << std::endl;
  }
}

void Server::clearClient(int fd) {
  closeClient(fd);

  for (size_t i = 0; i < _pollFds.size(); i++) {
    if (_pollFds[i].fd == fd) {
      _pollFds.erase(_pollFds.begin() + i);
      break;
    }
  }

  _clients.erase(fd);
}
/*  Commands management */

void Server::handleCommand(const std::string &command,
                           std::string &argument, int fd) {
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
  } else if (command == "PRIVMSG") {
    // Envoyer un message privé
  } else if (command == "QUIT") {
    // Déconnecter le client
  } else if (command == "PING") {
    // client.sendNumericReply(1, "PONG");
  } else if (command == "PASS" || command == "USER") {
    // if (argument.empty())
    //   sendNumericReply(461, ERR_NEEDMOREPARAMS);
    // else
    //   sendNumericReply(462, ERR_ALREADYREGISTERED);
  } else {
    // Commande inconnue
  }
}

// void Server::joinChannel(std::string &channelName, int fd) {
//     channelName = channelName.substr(1);

//     if (_channels.find(channelName) == _channels.end()) {
//         Channel newChannel(channelName);
//         _channels[channelName] = newChannel;
//     }

//     // Récupérer l'instance du client avant de l'accepter dans le canal
//     const Client &client = getClientByFd(fd);
//     _channels[channelName].acceptClientInTheChannel(client);

//     // Envoyer la réponse JOIN au client
//     // client._nick = "faboussa"; //
//     std::cout << "Client " << client.getNickName() << " joined channel " << channelName << std::endl;

//     std::string nick = client.getNickName();
//     #ifdef DEBUG
//       std::cout << "Client " << nick << " joined channel " << channelName << std::endl;
//     #endif
//     std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
//     send(fd, joinMessage.c_str(), joinMessage.length(), 0);

//     // Préparer et envoyer la liste des utilisateurs dans le canal (353 RPL_NAMREPLY)
//     std::string nameReply = ":" + SRV_NAME + " 353 " + nick + " = " + channelName + " :";
    
//     const clientsMap &clientsInChannel = _channels[channelName].getClientsInChannel();
//     for (clientsMap::const_iterator it = clientsInChannel.begin(); it != clientsInChannel.end(); ++it) {
//         nameReply += getClientByFd(it->first).getNickName() + " ";
//     }

//     // Terminer le message de liste avec un retour à la ligne
//     nameReply += "\r\n";
//     send(fd, nameReply.c_str(), nameReply.length(), 0);

//     // Envoyer le RPL_ENDOFNAMES (366) pour indiquer que la liste des noms est terminée
//     std::string endOfNames = ":" + SRV_NAME + " 366 " + nick + " " + channelName + " :End of /NAMES list\r\n";
//     send(fd, endOfNames.c_str(), endOfNames.length(), 0);

//     // Informer les autres clients dans le canal que quelqu'un a rejoint
//     for (clientsMap::const_iterator it = clientsInChannel.begin(); it != clientsInChannel.end(); ++it) {
//         if (it->first != fd) { // Évitez d'envoyer au client qui a rejoint
//             send(it->first, joinMessage.c_str(), joinMessage.length(), 0);
//         }
//     }
// }

