/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/28 14:25:32 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

#include "../includes/Parser.hpp"
#include "../includes/colors.hpp"
#include "../includes/utils.hpp"

bool Server::_signal = false;

Server::Server(int port, std::string password) {
  _port = port;
  _password = password;
  _signal = false;
  _socketFd = -1;
}

const Client &Server::getClientByFd(int fd) const {
  clientsMap::const_iterator it = _clients.find(fd);
  if (it == _clients.end()) {
    std::cerr << "Client not found with the given file descriptor" << std::endl;
  }
  return it->second;
}

/* Server Mounting */

void Server::runServer() {
  createSocket();
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

void Server::handleInitialMessage(Client &client, const std::string &message) {
  commandVectorPairs splittedPair = Parser::parseCommandIntoPairs(message);
  size_t vecSize = splittedPair.size();

  for (size_t it = 0; it < vecSize; ++it) {
    std::string command = splittedPair[it].first;
    std::string argument = splittedPair[it].second;
    std::cout << MAGENTA "Command: " << command << std::endl;
    std::cout << "Message: " << argument << RESET << std::endl;

    if (command == "CAP") {
      continue;
    } else if (command == "PASS") {
      if (argument != _password) {
        std::cout << RED "Invalid password" RESET << std::endl;
        clearClient(client.getFd());
        return;
      } else {
        client.declarePasswordGiven();
      }
    } else if (command == "NICK") {
      std::cout << MAGENTA "NICK" RESET << std::endl;
      if ((splittedPair[it].second).empty() ||
          Parser::verifyNick(argument, _clients) == false) {
        std::cout << BLUE "Invalid nickname" RESET << std::endl;
        clearClient(client.getFd());
        return;
      } else {
        std::cout << GREEN "Valid nickname : " << argument << RESET
                  << std::endl;
        client.setNickname(argument);
      }
    } else if (command == "USER") {
    } else {
      std::cout << CYAN "What the fuck ?" RESET << std::endl;
    }
  }
  if (client.isNicknameSet()) {
    std::cout << GREEN "NICKNAME SET" RESET << std::endl;
    if (client.isPasswordGiven())
      client.declareAccepted();
    else {
      std::cerr << RED "NO PASSWORD GIVEN !" RESET << std::endl;
      clearClient(client.getFd());
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
    std::cout << MAGENTA "Message count === " << client.getMessageCount()
              << RESET << std::endl;
    std::istringstream iss(message);
    std::string command;
    iss >> command;
    if (command == "JOIN")
      handleCommand(command, fd);
    else
      sendToAllClients(message);
  }
}

void Server::acceptNewClient() {
  Client cli;
  struct sockaddr_in cliadd;
  struct pollfd newPoll;
  socklen_t len = sizeof(cliadd);

  int newClientFd = accept(_socketFd, (sockaddr *)&cliadd, &len);
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

  _clients[newClientFd] = cli;
  _pollFds.push_back(newPoll);

  std::cout << GREEN "New client connected: " RESET << newClientFd << std::endl;
}

void Server::sendToAllClients(const std::string &message) {
  for (clientsMap::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    it->second.receiveMessage(message);
  }
}

void Server::closeClient(int fd) {
  // Fermer le socket du client
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
/* Chat Commands */

void Server::handleCommand(const std::string &command, int fd) {
  static_cast<void>(fd);
  if (command.empty()) {
    return;
  } else if (command == "JOIN") {
    // std::string channelName;
    // command >> channelName;
    // if (_channels.find(channelName) == _channels.end()) {
    //   _channels[channelName] = Channel(channelName);
    // }
    // _channels[channelName].acceptClientInTheChannel(_clients[fd]);
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
    // change nickname
  } else if (command == "PRIVMSG") {
    // Envoyer un message privé
  } else if (command == "QUIT") {
    // Déconnecter le client
  } else if (command == "PING") {
    // client.sendNumericReply(1, "PONG");
  } else {
    // Commande inconnue
  }
}

// void Server::handlePassword(int fd) {
//   char buffer[1024] = {0};
//   std::memset(buffer, 0, sizeof(buffer));
//   int valread = recv(fd, buffer, sizeof(buffer), 0);

//   switch (valread) {
//     case -1:
//       std::cerr << RED "Error while receiving message" RESET << std::endl;
//       // fallthrough
//     case 0:
//       clearClient(fd);
//       return;
//   }
//   std::cout << YELLOW << buffer << RESET << std::endl;
//   std::string message(buffer, valread);
//   std::istringstream iss(message);
//   sendToAllClients(message);
// }
