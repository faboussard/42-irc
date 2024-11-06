/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/06 17:50:03 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"
#include "../includes/serverConfig.hpp"

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Channel::Channel(const std::string &name) : _name(name), _key(""), _limit(0) {
  time_t now = time(0);
  _creationTime = toString(now);

  _type = toString(REG_CHAN);
  _nameWithPrefix = _type + _name;
  _mode.inviteOnly = false;
  _mode.topicSettableByOpsOnly = false;
  _mode.keyRequired = false;
  _mode.limitSet = false;
  _topic.topic = "";
  _topic.author = "";
  _topic.setTime = "";
}

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

const std::string &Channel::getName(void) const { return _name; }

const std::string &Channel::getType(void) const { return (_type); }

const std::string Channel::getNameWithPrefix(void) const {
  return (_nameWithPrefix);
}

const std::string &Channel::getCreationTime(void) const {
  return _creationTime;
}

const clientPMap &Channel::getChannelClients(void) const {
  return (_channelClients);
}

const clientPMap &Channel::getChannelOperators(void) const {
  return (_channelOperators);
}

const Topic &Channel::getTopic(void) const { return _topic; }

const Mode &Channel::getMode(void) const { return _mode; }

const std::string Channel::getChannelModeFlag(void) const {
  std::string flags = "+";
  if (_mode.inviteOnly) flags += "i";
  if (_mode.topicSettableByOpsOnly) flags += "t";
  if (_mode.keyRequired) flags += "k";
  if (_mode.limitSet) flags += "l";
  return (flags);
}

const std::string &Channel::getKey(void) const { return (_key); }

int Channel::getLimit(void) const { return (_limit); }

/*============================================================================*/
/*       Setters                                                              */
/*============================================================================*/

void Channel::setTopic(const std::string &topic, const std::string &author) {
  _topic.topic = topic;
  _topic.author = author;
  time_t now = time(0);
  _topic.setTime = toString(now);
}

/*============================================================================*/
/*                                                                            */
/*============================================================================*/

void Channel::removeClientFromTheChannel(int fd) {
  if (_channelClients.find(fd) != _channelClients.end()) {
    _channelClients[fd]->receiveMessage(
        "You have been removed from the channel");
    _channelClients.erase(fd);
    std::cout << "Client " << fd << " removed from channel " << _name
              << std::endl;
  } else {
    std::cerr << RED "Client " RESET << fd << " not found in channel " << _name
              << RESET << std::endl;
  }
}

void Channel::addClientToChannelMap(Client *client) {
  _channelClients[client->getFd()] = client;
  std::cout << "Client " << client->getFd() << " added to channel " << _name
            << std::endl;
}


void Channel::addOperatorsToChannelMap(Client *client) {
  _channelClients[client->getFd()] = client;
  std::cout << "Operator " << client->getFd() << " added to channel " << _name
            << std::endl;
}

void Channel::receiveMessageInTheChannel(int fd) {
  if (_channelClients.find(fd) != _channelClients.end()) {
    std::string message = _channelClients[fd]->shareMessage();
    if (!message.empty()) {
      std::cout << "Message received in channel " << _name << " from client "
                << fd << ": " << message << std::endl;
      clientPMap::iterator itBegin = _channelClients.begin();
      clientPMap::iterator itEnd = _channelClients.end();
      for (clientPMap::iterator it = itBegin; it != itEnd; ++it) {
        if (it->first != fd) {
          it->second->receiveMessage(message);
        }
      }
    }
  }
}

/*============================================================================*/
/*       Modes handling                                                       */
/*============================================================================*/

/* invite-only (i) */
void Channel::activateInviteOnlyMode(void) {
  _mode.inviteOnly = true;
  std::cout << "[" << _nameWithPrefix << "] Invite-only mode activated"
            << std::endl;
}

void Channel::deactivateInviteOnlyMode(void) {
  _mode.inviteOnly = false;
  std::cout << "[" << _nameWithPrefix << "] Invite-only mode desactivated"
            << std::endl;
}

/* topic-settable-by-ops-only (t) */
void Channel::activateTopicOpsOnlyMode(void) {
  _mode.topicSettableByOpsOnly = true;
  std::cout << "[" << _nameWithPrefix
            << "] Topic settable by Operator only mode activated" << std::endl;
}

void Channel::deactivateTopicOpsOnlyMode(void) {
  _mode.topicSettableByOpsOnly = false;
  std::cout << "[" << _nameWithPrefix
            << "] Topic settable by Operator only mode desactivated"
            << std::endl;
}

/* key-mode (k) */
void Channel::updateKey(const std::string &newKey) {
  _key = newKey;
  std::cout << "[" << _nameWithPrefix << "] Channel key updated." << std::endl;
}

void Channel::activateKeyMode(const std::string &key, const Client &client) {
  if (key.empty()) {
    send461NeedMoreParams(client, "MODE");
  } else {
    _mode.keyRequired = true;
    _key = key;
    std::cout << "[" << _nameWithPrefix << "] Key mode activated" << std::endl;
  }
}
void Channel::deactivateKeyMode(void) {
  _mode.keyRequired = false;
  _key = "";
  std::cout << "[" << _nameWithPrefix << "] Key mode desactivated" << std::endl;
}

/* add/remove operator (o) */

void Channel::addOperator(Client *client) {
  _channelOperators[client->getFd()] = client;
  std::cout << "[" << _nameWithPrefix << "]" << "Client " << client->getFd()
            << " added as operator in channel " << _name << std::endl;
}

void Channel::removeOperator(Client *client) {
  _channelOperators.erase(client->getFd());
  std::cout << "[" << _nameWithPrefix << "]" << "Client " << client->getFd()
            << " removed from operator in channel " << _name << std::endl;
}

/* limit-mode (l) */
void Channel::activateLimitMode(int limit, const Client &client) {
  if (limit == 0) {
    send461NeedMoreParams(client, "MODE");
  } else {
    _mode.limitSet = true;
    _limit = limit;
    std::cout << "[" << _nameWithPrefix << "] Limit mode activated"
              << std::endl;
  }
}

void Channel::deactivateLimitMode(void) {
  _mode.limitSet = false;
  _limit = 0;
  std::cout << "[" << _nameWithPrefix << "] Limit mode desactivated"
            << std::endl;
}
