/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/23 08:44:27 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

#include "../includes/colors.hpp"

Channel::Channel(const std::string& name) : _name(name) {}

void Channel::removeClientFromTheChannel(int fd) {
  if (_clients.find(fd) != _clients.end()) {
    _clients[fd].sendMessage("You have been removed from the channel");

    _clients.erase(fd);
    std::cout << "Client " << fd << " removed from channel " << _name
              << std::endl;
  } else {
    std::cerr << RED "Client " << fd << " not found in channel " << _name
              << RESET << std::endl;
  }
}

void Channel::acceptClientInTheChannel(const Client& client) {
  _clients[client.getFd()] = client;
  std::cout << "Client " << client.getFd() << " added to channel " << _name
            << std::endl;
}

void Channel::receiveMessageInTheChannel(int fd) {
  if (_clients.find(fd) != _clients.end()) {
    std::string message = "coucou";  //_clients[fd].receiveMessage();
    if (!message.empty()) {
      std::cout << "Message received in channel " << _name << " from client "
                << fd << ": " << message << std::endl;
      // Diffuser le message à tous les autres clients dans le canal
      std::map<int, Client>::iterator itBegin = _clients.begin();
      std::map<int, Client>::iterator itEnd = _clients.end();
      for (std::map<int, Client>::iterator it = itBegin; it != itEnd; ++it) {
        if (it->first != fd) {
          it->second.sendMessage(message);
        }
      }
    }
  }
}

void Channel::setTopic(const std::string& topic) {
  _topic = topic;
}

std::string Channel::getTopic() const {
  return (_topic);
}