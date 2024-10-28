/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/28 14:55:37 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"

Channel::Channel(const std::string &name) : _name(name) {
  _mode.inviteOnly = false;
  _mode.topicSettableByOpsOnly = false;
  _mode.keyRequired = false;
  _mode.limitSet = false;
  _mode.key = "";
  _mode.limit = 0;
}

/*------ Getters ------------------------------------------------------------ */

const std::string &Channel::getName() const { return _name; }

const std::string Channel::getNameWithPrefix() const {
  if (_mode.keyRequired)
    return ("&" + _name);
  else
    return ("#" + _name);
}

const clientsMap &Channel::getClientsInChannel() const {
  return _clientsInChannel;
}

const Topic &Channel::getTopic() const { return _topic; }

const Mode &Channel::getMode() const { return _mode; }

const std::string &Channel::getKey(void) const { return (_mode.key); }

int Channel::getLimit(void) const { return (_mode.limit); }

/*------ Setters ------------------------------------------------------------ */

// void Channel::setTopic(const std::string &topic) { _topic = topic; }
void Channel::setTopic(const std::string &topic, const std::string &author) {
  _topic.topic = topic;
  _topic.author = author;
  time_t now = time(0);
  _topic.setTime = toString(now);
}

void Channel::setInviteOnlyMode(void) {
  if (_mode.inviteOnly == true) {
    _mode.inviteOnly = false;
  } else {
    _mode.inviteOnly = true;
  }
}

void Channel::setTopicSettableByOpsOnlyMode(void) {
  if (_mode.topicSettableByOpsOnly == true) {
    _mode.topicSettableByOpsOnly = false;
  } else {
    _mode.topicSettableByOpsOnly = true;
  }
}

/*--------------------------------------------------------------------------- */
// Supprime un client du canal
void Channel::removeClientFromTheChannel(int fd) {
  if (_clientsInChannel.find(fd) != _clientsInChannel.end()) {
    _clientsInChannel[fd].receiveMessage(
        "You have been removed from the channel");
    _clientsInChannel.erase(fd);
    std::cout << "Client " << fd << " removed from channel " << _name
              << std::endl;
  } else {
    std::cerr << RED "Client " RESET << fd << " not found in channel " << _name
              << RESET << std::endl;
  }
}

void Channel::acceptClientInTheChannel(const Client &client) {
  _clientsInChannel[client.getFd()] = client;
  std::cout << "Client " << client.getFd() << " added to channel " << _name
            << std::endl;
}

void Channel::receiveMessageInTheChannel(int fd) {
  if (_clientsInChannel.find(fd) != _clientsInChannel.end()) {
    std::string message = _clientsInChannel[fd].shareMessage();
    if (!message.empty()) {
      std::cout << "Message received in channel " << _name << " from client "
                << fd << ": " << message << std::endl;
      clientsMap::iterator itBegin = _clientsInChannel.begin();
      clientsMap::iterator itEnd = _clientsInChannel.end();
      for (clientsMap::iterator it = itBegin; it != itEnd; ++it) {
        if (it->first != fd) {
          it->second.receiveMessage(message);
        }
      }
    }
  }
}

void Channel::activateKeyMode(const std::string &key, const Client &cli) {
  if (key.empty())
    send461NeedMoreParams(cli.getFd(), cli.getNickName(), "MODE");
  else {
    _mode.keyRequired = true;
    _mode.key = key;
  }
}
void Channel::deactivateKeyMode(void) {
  _mode.keyRequired = false;
  _mode.key = "";
}

void Channel::activateLimitMode(int limit, const Client &cli) {
  if (limit == 0)
    send461NeedMoreParams(cli.getFd(), cli.getNickName(), "MODE");
  else {
    _mode.limitSet = true;
    _mode.limit = limit;
  }
}

void Channel::deactivateLimitMode(void) {
  _mode.limitSet = false;
  _mode.limit = 0;
}
