/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/26 23:20:28 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

#include "../includes/colors.hpp"
#include "../includes/utils.hpp"

// Constructeur de la classe Channel
Channel::Channel(const std::string &name) : _name(name) {}

const std::string &Channel::getName() const { return _name; }
const clientsMap &Channel::getClientsInChannel() const { return _clientsInChannel; }
// const std::string &Channel::getTopic() const { return _topic; }
const Topic &Channel::getTopic() const { return _topic; }

// void Channel::setTopic(const std::string &topic) { _topic = topic; }
void Channel::setTopic(const std::string &topic, const std::string &author) {
  _topic.topic = topic;
  _topic.author = author;
  time_t now = time(0);
  _topic.setTime = toString(now);
}

// Supprime un client du canal
void Channel::removeClientFromTheChannel(int fd) {
  if (_clientsInChannel.find(fd) != _clientsInChannel.end()) {
    _clientsInChannel[fd].receiveMessage("You have been removed from the channel");
    _clientsInChannel.erase(fd);
    std::cout << "Client " << fd << " removed from channel " << _name
              << std::endl;
  } else {
    std::cerr << RED "Client " RESET << fd << " not found in channel " << _name
              << RESET << std::endl;
  }
}

// Accepte un client dans le canal
void Channel::acceptClientInTheChannel(Client &client) {
  _clientsInChannel[client.getFd()] = client;
  std::cout << "Client " << client.getFd() << " added to channel " << _name
            << std::endl;
}

// Reçoit un message d'un client dans le canal
void Channel::receiveMessageInTheChannel(int fd) {
  if (_clientsInChannel.find(fd) != _clientsInChannel.end()) {
    std::string message = _clientsInChannel[fd].shareMessage();
    if (!message.empty()) {
      std::cout << "Message received in channel " << _name << " from client "
                << fd << ": " << message << std::endl;
      std::map<int, Client>::iterator itBegin = _clientsInChannel.begin();
      std::map<int, Client>::iterator itEnd = _clientsInChannel.end();
      for (std::map<int, Client>::iterator it = itBegin; it != itEnd; ++it) {
        if (it->first != fd) {
          it->second.receiveMessage(message);
        }
      }
    }
  }
}
