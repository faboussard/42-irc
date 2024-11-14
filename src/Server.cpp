/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/14 14:08:05 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

#include <iostream>
#include <string>

#include "../includes/Parser.hpp"
#include "../includes/colors.hpp"
#include "../includes/utils.hpp"

bool Server::_signal = false;

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Server::Server(int port, const std::string &password)
    : _socketFd(-1), _port(port), _password(password) {
  _signal = false;
}

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

// Client &Server::findClientByFd(int fd) {
//   clientsMap::iterator it = _clients.find(fd);
//   if (it == _clients.end()) {
//     std::cerr << "Client not found with the given file descriptor"
//     << std::endl;
//     throw std::runtime_error("Client not found");
//   }
//   return it->second;
// }

// const clientsMap &Server::getClients() const { return _clients; }

// const channelsMap &Server::getChannels() const { return _channels; }

Channel *Server::findChannelByName(const std::string &name) {
  channelsMap::iterator it = _channels.find(name);
  if (it == _channels.end())
   return (NULL);
  return (&it->second);
}

Client *Server::findClientByNickname(const std::string &nickname) {
  clientsMap::iterator itEnd = _clients.end();
  for (clientsMap::iterator it = _clients.begin(); it != itEnd; ++it) {
    if (it->second.getNickname() == nickname) {
      return &it->second;
    }
  }
  return NULL;
}

// const std::string &Server::getPassword(void) const { return _password; }

// int Server::getPort(void) const { return _port; }

// int Server::getSocketFd(void) const { return _socketFd; }

/*============================================================================*/
/*       Server Mounting                                                      */
/*============================================================================*/

void Server::runServer(void) {
  createSocket();
  fetchStartTime();
  std::cout << GREEN "Server started on port " << _port << " at " << _startTime
            << RESET << std::endl;
  acceptAndChat();
}

void Server::createSocket(void) {
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
  char buffer[26];

  ctime_r(&now, buffer);
  std::string startTime = buffer;
  startTime.erase(startTime.find_last_not_of("\n") + 1);
  _startTime = startTime;
}

void Server::acceptAndChat(void) {
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

void Server::closeServer(void) {
  clientsMap::iterator itEnd = _clients.end();
  for (clientsMap::iterator it = _clients.begin(); it != itEnd; ++it) {
    closeClient(it->second.getFd());
  }
  _clients.clear();
  _channels.clear();
  // Fermer le socket principal
  if (_socketFd != -1) {
    std::cout << RED "Server <" RESET << _socketFd << RED "> Disconnected" RESET
              << std::endl;
    close(_socketFd);
    _socketFd = -1;
  }
  _pollFds.clear();
  shrink_to_fit(&_pollFds);
  _channels.clear();
  delete gConfig;
}

/*============================================================================*/
/*       Clients connection Management                                        */
/*============================================================================*/

void Server::acceptNewClient(void) {
  struct sockaddr_in cliadd;
  socklen_t len = sizeof(cliadd);
  struct pollfd newPoll;

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

  std::string clientIp = inet_ntoa(cliadd.sin_addr);
  struct hostent *host =
      gethostbyaddr(&cliadd.sin_addr, sizeof(cliadd.sin_addr), AF_INET);
  std::string hostName;
  if (host->h_name == NULL || sizeof(host->h_name) == 0 ||
      static_cast<size_t>(host->h_length) > gConfig->getLimit(HOSTLEN))
    hostName = clientIp;
  else
    hostName = host->h_name;
  Client cli(newClientFd, clientIp, hostName);
#ifdef DEBUG
  std::cout << "New client IP " << cli.getIp() << std::endl;
  std::cout << "New client host name " << cli.getHostName() << std::endl;
#endif
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
  // if (_clients.at(fd).getChannelsCount() > 0) {  // Decommente after merge
  // join & part
  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it) {
    if (it->second.getChannelClients().find(fd) !=
        it->second.getChannelClients().end())
      it->second.checkAndremoveClientFromTheChannel(fd);
    if (it->second.getChannelOperators().find(fd) !=
        it->second.getChannelOperators().end())
      it->second.removeOperator(&_clients.at(fd));
    if (it->second.getInvitedClients().find(fd) !=
        it->second.getInvitedClients().end())
      it->second.removeClientFromInvitedMap(&_clients.at(fd));
  }
  _clients.erase(fd);
}

void Server::sendConnectionMessage(const Client &client) const {
  std::string nick = client.getNickname();
  std::string user = client.getUserName();
  std::string host = client.getHostName();
  int fd = client.getFd();
  sendWelcome(fd, nick);
  send001Welcome(fd, nick, user, host);
  send002Yourhost(fd, nick);
  send003Created(fd, nick, _startTime);
  send104Myinfo(fd, nick);
  send005Isupport(fd, nick);
}

/*============================================================================*/
/*       Clients management                                                   */
/*============================================================================*/

// void Server::sendToAllClients(const std::string &message) {
//   for (clientsMap::iterator it = _clients.begin(); it != _clients.end();
//   ++it) {
//     if (it->second.isAccepted()) it->second.receiveMessage(message);
//   }
// }

void Server::broadcastInChannel(const Client &client, const Channel &channel,
                                const std::string &command,
                                const std::string &content) {
  std::string message = ":" + client.getNickname() + " " + command + " " +
                        channel.getNameWithPrefix() + " :" + content + "\r\n";
  const clientPMap &allClients = channel.getChannelClients();
  clientPMap::const_iterator itEnd = allClients.end();
  for (clientPMap::const_iterator it = allClients.begin(); it != itEnd; ++it) {
    it->second->receiveMessage(message);
  }
}

bool Server::clientExists(const std::string &nick) const {
  clientsMap::const_iterator itEnd = _clients.end();
  for (clientsMap::const_iterator it = _clients.begin(); it != itEnd; ++it) {
    if (it->second.getNickname() == nick) return (true);
  }
  return (false);
}
