/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/17 11:53:10 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

#include "../includes/colors.hpp"

bool Server::_signal = false;

Server::Server(int port) : _port(port) {
  _signal = false;
  _socketFd = -1;
}

void Server::runServer() {
  createSocket();
  std::cout << GREEN << "Server started on port " << _port << RESET
            << std::endl;
  while (!_signal) {
    int pollResult = poll(_pollFds.data(), _pollFds.size(), -1);
    // Utilisation d'un seul appel à poll

    if (pollResult == -1) {
      std::cerr << RED "Error while polling" RESET << std::endl;
      break;
    }

    for (size_t i = 0; i < _pollFds.size(); ++i) {
      if (_pollFds[i].revents & POLLIN) {
        if (_pollFds[i].fd == _socketFd) {
          acceptNewClient();
        } else {
          handleClientMessage(_pollFds[i].fd);
        }
      }
    }
  }
}

void shrink_to_fit(std::vector<struct pollfd>& vec) {
  std::vector<struct pollfd>(vec).swap(vec);
}

void Server::closeServer() {
  for (size_t i = 0; i < _clients.size(); ++i) {
    std::cout << RED << "Client <" << _clients[i].getFd();
    std::cout << "> Disconnected" << RESET << std::endl;
    close(_clients[i].getFd());
  }
  _clients.clear();
  if (_socketFd != -1) {
    std::cout << RED << "Server <" << _socketFd;
    std::cout << "> Disconnected" << RESET << std::endl;
    close(_socketFd);
    _socketFd = -1;
  }
  for (size_t i = 0; i < _pollFds.size(); ++i) {
    if (_pollFds[i].fd != -1) {
      close(_pollFds[i].fd);
    }
  }
  _pollFds.clear();
  shrink_to_fit(_pollFds);
}

void Server::signalHandler(int signal) {
  if (signal == SIGINT || signal == SIGQUIT) {
    std::cout << std::endl << "Signal Received!" << std::endl;
    Server::_signal = true;
  }
}

void Server::clearClient(int fd) {
  for (size_t i = 0; i < _pollFds.size(); i++) {
    if (_pollFds[i].fd == fd) {
      _pollFds.erase(_pollFds.begin() + i);
      break;
    }
  }
  for (size_t i = 0; i < _clients.size(); i++) {
    if (_clients[i].getFd() == fd) {
      _clients.erase(_clients.begin() + i);
      break;
    }
  }
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
  if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket");

  if (fcntl(_socketFd, F_SETFL, O_NONBLOCK) == -1)
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket");

  if (bind(_socketFd, (struct sockaddr*)&_address, sizeof(_address)) == -1)
    throw std::runtime_error("Failed to bind socket");

  if (listen(_socketFd, 10) == -1)
    throw std::runtime_error("Failed to listen on socket");

  struct pollfd newPoll;
  newPoll.fd = _socketFd;
  newPoll.events = POLLIN;
  newPoll.revents = 0;
  _pollFds.push_back(newPoll);
}

void Server::handleCommand(const std::string& command, int fd) {
  static_cast<void>(fd);
  if (command.empty()) {
    return;
  } else if (command == "JOIN") {
    // Ajouter le client au canal
  } else if (command == "PART") {
    // Retirer le client du canal
  } else if (command == "LIST") {
    // Lister les canaux
  } else if (command == "TOPIC") {
    // Changer le sujet du canal
  } else if (command == "NAMES") {
    // Lister les clients dans le canal
  } else if (command == "KICK") {
    // Exclure un client du canal
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

void Server::handleClientMessage(int fd) {
  char buffer[1024] = {0};
  std::memset(buffer, 0, sizeof(buffer));
  int valread = recv(fd, buffer, sizeof(buffer), 0);

  switch (valread) {
    case -1:
      std::cerr << RED "Error while receiving message" RESET << std::endl;
    case 0:
      clearClient(fd);
      return;
  }

  std::string message(buffer, valread);
  std::cout << "Received message from client " << fd << ": " << message
            << std::endl;
  // Traitez le message ici
  // Exemple : Si le message est "JOIN #channel", ajoutez le client au canal
  std::istringstream iss(message);
  std::string command;
  iss >> command;

  if (command == "JOIN") {
    std::string channelName;
    iss >> channelName;
    if (_channels.find(channelName) == _channels.end()) {
      _channels[channelName] = Channel(channelName);
    }
    _channels[channelName].acceptClientInTheChannel(_clients[fd]);
  } else {
    // Diffusez le message dans le canal approprié
    sendToAllClients(message);
  }
}

void Server::acceptNewClient() {
  int newClientFd = accept(_socketFd, NULL, NULL);
  if (newClientFd == -1) {
    std::cerr << "Failed to accept new client" << std::endl;
    return;
  }

  struct pollfd newPoll;
  newPoll.fd = newClientFd;
  newPoll.events = POLLIN;  // Surveiller les événements de lecture
  newPoll.revents = 0;
  _pollFds.push_back(newPoll);
  _clients.push_back(Client(newClientFd));

  std::cout << "New client connected: " << newClientFd << std::endl;
}

void Server::sendToAllClients(const std::string& message) {
  for (size_t i = 0; i < _clients.size(); ++i) {
    _clients[i].receiveMessage(message);
  }
}