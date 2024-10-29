/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/29 14:33:42 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

#include "../includes/colors.hpp"
#include "../includes/utils.hpp"

bool Server::_signal = false;

Server::Server(int port, const std::string &password) {
  _port = port;
  _password = password;
  _signal = false;
  _socketFd = -1;
  _name = "coco";
}

/* -------------------------  Getters ----------------------------------- */

const Client &Server::getClientByFd(int fd) const {
  clientsMap::const_iterator it = _clients.find(fd);
  if (it == _clients.end()) {
    std::cerr << "Client not found with the given file descriptor" << std::endl;
    throw std::runtime_error("Client not found");
  }
  return it->second;
}

const clientsMap &Server::getClients() const { return _clients; }

const channelsMap &Server::getChannels() const { return _channels; }


const Channel &Server::getChannelByName(const std::string &name) const {
  channelsMap::const_iterator it = _channels.find(name);
  if (it == _channels.end()) {
    std::cerr << "Channel not found with the given name" << std::endl;
    throw std::runtime_error("Channel not found");
  }
  return it->second;
}

const std::string &Server::getServerName() const { return _name; }

const std::string &Server::getPassword() const { return _password; }

int Server::getPort() const { return _port; }

int Server::getSocketFd() const { return _socketFd; }

std::string Server::getChannelSymbol(const std::string &channelName) const {
    channelsMap::const_iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        if (it->second.isSecret) {
            return SECRET_CANAL;
        } else {
            return PUBLIC_CANAL;
        }
    }
    return PUBLIC_CANAL;
}


std::string Server::getUserPrefix(const Client &client) const{
    if (client.isOperator) {
        return "@"; // Préfixe opérateur
    }
    return ""; // Pas de préfixe pour les membres réguliers
}

/* -------------------------  Member functions ----------------------------------- */


/* Server Mounting */

void Server::runServer() {
  createSocket();
  setStartTime();
  std::cout << GREEN "Server started on port " RESET << _port << std::endl;
  acceptAndChat();
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
        "Failed to bind socket. Port might be used elsewere");
  }

  if (listen(_socketFd, SOMAXCONN) == -1) {
    throw std::runtime_error("Failed to listen on socket");
  }
}

void Server::setStartTime(void) {
  time_t now = time(0);
  _startTime = ctime(&now);
  _startTime.erase(_startTime.end() - 1);
}

void Server::acceptAndChat() {
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
    case 0:
      std::cout << "Client " << fd << " disconnected" << std::endl;
      clearClient(fd);
      return;
  }

  std::string message(buffer, valread);
  std::cout << "Received message from client " << fd << ": " << message
            << std::endl;
  std::string command = message.substr(0, message.find(" "));
  std::string params = message.substr(message.find(" ") + 1);
  handleCommand(command, params, fd);
  // sendToAllClients(message); //dans le channel, si pas de command, ca envoie
  // a tout le monde sous la forme de PRIVMSG
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
  cli.setIp(inet_ntoa(cliadd.sin_addr));  // inet_ntoa = convertit l'adresse IP
                                          // en une chaîne de caractères

  // ----- For test ---------
  cli.setNickName("kitten");
  cli.setUserName("kitten");
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
    it->second.receiveMessage(message);
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

 void Server::addClient(int fd, const Client &client) {
        _clients[fd] = client; // This will now be accessible
    }
/*  Commands management */

void Server::handleCommand(std::string &command, std::string &params, int fd) {
  static_cast<void>(fd);
  if (command.empty()) {
    return;
  } else if (command == "JOIN") {
  #ifdef DEBUG
    std::cout << GREY "JOIN command received" RESET << std::endl;
    std::cout << GREY "Params: " RESET << params << std::endl;
  #endif
    if (params == "#")
      joinChannel(params, fd); // ajout condition et du & 
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

// void Server::joinChannel(std::string &channelName, int fd) {
//     channelName = channelName.substr(1);

//     if (_channels.find(channelName) == _channels.end()) {
//         Channel newChannel(channelName);
//         _channels[channelName] = newChannel;
//     }
//     const Client &client = getClientByFd(fd);
//     _channels[channelName].acceptClientInTheChannel(client);
//     std::string nick = client.getNickName();
//     #ifdef DEBUG
//       std::cout << GREY "Client " << nick << " joined channel " RESET << channelName << std::endl;
//     #endif
//     std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
//     send(fd, joinMessage.c_str(), joinMessage.length(), 0);
//     // Préparer et envoyer la liste des utilisateurs dans le canal (353 RPL_NAMREPLY)
//     std::string nameReply = ":" + getServerName() + " 353 " + nick + " = " + channelName + " :";
    
//     const clientsMap &clientsInChannel = _channels[channelName].getClientsInChannel();
//     for (clientsMap::const_iterator it = clientsInChannel.begin(); it != clientsInChannel.end(); ++it) {
//         nameReply += getClientByFd(it->first).getNickName() + " ";
//     }
//     nameReply += "\r\n";
//     send(fd, nameReply.c_str(), nameReply.length(), 0);
//     // Envoyer le RPL_ENDOFNAMES (366) pour indiquer que la liste des noms est terminée
//     std::string endOfNames = ":" + getServerName() + " 366 " + nick + " " + channelName + " :End of /NAMES list\r\n";
//     send(fd, endOfNames.c_str(), endOfNames.length(), 0);
//     for (clientsMap::const_iterator it = clientsInChannel.begin(); it != clientsInChannel.end(); ++it) {
//         if (it->first != fd) {
//             send(it->first, joinMessage.c_str(), joinMessage.length(), 0);
//         }
//     }
// }

// void Server::handlePassword(int fd) {
//   char buffer[1024] = {0};
//   std::memset(buffer, 0, sizeof(buffer));
//   int valread = recv(fd, buffer, sizeof(buffer), 0);

//   switch (valread) {
//     case -1:
//       std::cerr << RED "Error while receiving message" RESET << std::endl;
//     case 0:
//       clearClient(fd);
//       return;
//   }
//   std::cout << YELLOW << buffer << RESET << std::endl;
//   std::string message(buffer, valread);
//   std::istringstream iss(message);
//   sendToAllClients(message);
// }
