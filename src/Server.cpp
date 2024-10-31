/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/31 16:08:36 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

#include "../includes/Parser.hpp"
#include "../includes/colors.hpp"
#include "../includes/utils.hpp"

bool Server::_signal = false;

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Server::Server(int port, std::string password) {
  _port = port;
  _password = password;
  _signal = false;
  _socketFd = -1;
}

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

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

const std::string &Server::getPassword() const { return _password; }

int Server::getPort() const { return _port; }

int Server::getSocketFd() const { return _socketFd; }

/*============================================================================*/
/*       Server Mounting                                                      */
/*============================================================================*/

void Server::runServer() {
  createSocket();
  std::cout << GREEN "Server started on port " RESET << _port << std::endl;
  fetchStartTime();
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

void Server::fetchStartTime(void) {
  time_t now = time(0);
  _startTime = ctime(&now);
  _startTime.erase(_startTime.find_last_not_of("\n") + 1);
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

void Server::signalHandler(int signal) {
  if (signal == SIGINT || signal == SIGQUIT) {
    _signal = true;
    std::cout << std::endl << "Signal Received!" << std::endl;
  }
}

/*============================================================================*/
/*       Server Shutdown                                                      */
/*============================================================================*/

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

/*============================================================================*/
/*       Clients connection Management                                        */
/*============================================================================*/

void Server::acceptNewClient() {
  Client cli;
  struct sockaddr_in cliadd;
  struct pollfd newPoll;
  socklen_t len = sizeof(cliadd);

  int newClientFd =
      accept(_socketFd, reinterpret_cast<sockaddr *>(&cliadd), &len);
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
  // testAllNumericReplies(_startTime, cli, "COMMAND", "puppy");
  // ------------------------

  _clients[newClientFd] = cli;
  _pollFds.push_back(newPoll);
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

/*============================================================================*/
/*       Clients management                                                   */
/*============================================================================*/

void Server::sendConnectionMessage(const Client &client) const {
  std::string nick = client.getNickName().empty() ? "*" : client.getNickName();
  std::string user = client.getUserName().empty() ? "*" : client.getUserName();
  std::string host = client.getIp().empty() ? "*" : client.getIp();
  int fd = client.getFd();
  sendWelcome(fd, nick);
  send001Welcome(fd, nick, user, host);
  send002Yourhost(fd, nick);
  send003Created(fd, nick, _startTime);
  send104Myinfo(fd, nick);
  send005Isupport(client);
}

void Server::sendToAllClients(const std::string &message) {
  for (clientsMap::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    if (it->second.isAccepted()) it->second.receiveMessage(message);
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
//     std::cout << "Client " << client.getNickName() << " joined channel " <<
//     channelName << std::endl;

//     std::string nick = client.getNickName();
//     #ifdef DEBUG
//       std::cout << "Client " << nick << " joined channel " << channelName <<
//       std::endl;
//     #endif
//     std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
//     send(fd, joinMessage.c_str(), joinMessage.length(), 0);

//     // Préparer et envoyer la liste des utilisateurs dans le canal (353
//     RPL_NAMREPLY) std::string nameReply = ":" + SRV_NAME + " 353 " + nick + "
//     = " + channelName + " :";

//     const clientsMap &clientsInChannel =
//     _channels[channelName].getClientsInChannel(); for
//     (clientsMap::const_iterator it = clientsInChannel.begin(); it !=
//     clientsInChannel.end(); ++it) {
//         nameReply += getClientByFd(it->first).getNickName() + " ";
//     }

//     // Terminer le message de liste avec un retour à la ligne
//     nameReply += "\r\n";
//     send(fd, nameReply.c_str(), nameReply.length(), 0);

//     // Envoyer le RPL_ENDOFNAMES (366) pour indiquer que la liste des noms
//     est terminée std::string endOfNames = ":" + SRV_NAME + " 366 " + nick + "
//     " + channelName + " :End of /NAMES list\r\n"; send(fd,
//     endOfNames.c_str(), endOfNames.length(), 0);

//     // Informer les autres clients dans le canal que quelqu'un a rejoint
//     for (clientsMap::const_iterator it = clientsInChannel.begin(); it !=
//     clientsInChannel.end(); ++it) {
//         if (it->first != fd) { // Évitez d'envoyer au client qui a rejoint
//             send(it->first, joinMessage.c_str(), joinMessage.length(), 0);
//         }
//     }
// }
