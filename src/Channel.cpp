/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/12/07 16:16:22 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

#include <iostream>
#include <string>

#include "../includes/Log.hpp"
#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"

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

const clientPMap &Channel::getInvitedClients(void) const {
  return (_invitedClients);
}

const std::set<std::string> &Channel::getBannedUsers(void) const {
  return (_bannedUsers);
}

const Topic &Channel::getTopic(void) const { return _topic; }

const Mode &Channel::getMode(void) const { return _mode; }

const std::string Channel::getChannelModeFlag(void) const {
  std::ostringstream flagsStream;
  flagsStream << "+";
  if (_mode.inviteOnly) flagsStream << "i";
  if (_mode.topicSettableByOpsOnly) flagsStream << "t";
  if (_mode.keyRequired) flagsStream << "k";
  if (_mode.limitSet) flagsStream << "l";
  return (flagsStream.str());
}

const std::string &Channel::getKey(void) const { return (_key); }

unsigned int Channel::getLimit(void) const { return (_limit); }

/*============================================================================*/
/*       Setters                                                              */
/*============================================================================*/

void Channel::setTopic(const std::string &topic, const std::string &author) {
  _topic.topic = topic;
  _topic.author = author;
  time_t now = time(0);
  _topic.setTime = toString(now);

  std::ostringstream oss;
  oss << _nameWithPrefix << ": Topic updated by " << author;
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

/*============================================================================*/
/*       Clients management                                                   */
/*============================================================================*/

void Channel::addClientToChannelMap(Client *client) {
  _channelClients[client->getFd()] = client;
  std::ostringstream oss;
  oss << _nameWithPrefix << ": " << client->getNickname() << " has joined";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

void Channel::removeClientFromChannelMap(Client *client) {
  _channelClients.erase(client->getFd());
  std::ostringstream oss;
  oss << _nameWithPrefix << ": " << client->getNickname() << " has left";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

void Channel::checkAndremoveClientFromTheChannel(int fd) {
  if (_channelClients.find(fd) != _channelClients.end()) {
    {
      std::ostringstream oss;
      oss << "Client " << fd << " removed from channel " << _name;
      Log::printLog(INFO_LOG, CHANNEL, oss.str());
    }
    _channelClients.erase(fd);
  }
}

void Channel::addClientToInvitedMap(Client *invited,
                                    const std::string &invitingNick) {
  _invitedClients[invited->getFd()] = invited;
  {
    std::ostringstream oss;
    invited->receiveMessage(FROM_SERVER + "NOTICE" + " " +
                            "You have been invited by " + invitingNick +
                            "\r\n");
  }
  {
    std::ostringstream oss;
    oss << _nameWithPrefix << ": " << invited->getNickname()
        << " has been invited by " << invitingNick;
    Log::printLog(INFO_LOG, CHANNEL, oss.str());
  }
}

void Channel::removeClientFromInvitedMap(Client *client) {
  {
    std::ostringstream oss;
    client->receiveMessage(FROM_SERVER + "NOTICE" + " " +
                           "You have been removed from the "
                           "invited clients list\r\n");
  }
  {
    std::ostringstream oss;
    oss << _nameWithPrefix << ": " << client->getNickname()
        << " has been removed from invited clients list";
    Log::printLog(INFO_LOG, CHANNEL, oss.str());
  }
  _invitedClients.erase(client->getFd());
}

void Channel::removeClientFromBannedList(const std::string &nickname) {
  _bannedUsers.erase(nickname);
  std::ostringstream oss;
  oss << _nameWithPrefix << ": User " << nickname << " has been unbanned.";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

bool Channel::isClientInChannel(int fd) const {
  if (_channelClients.find(fd) != _channelClients.end()) {
    return (true);
  }
  return (false);
}

bool Channel::isClientInvited(int fd) const {
  if (_invitedClients.find(fd) != _invitedClients.end()) {
    return (true);
  }
  return (false);
}

bool Channel::isClientInBannedList(const std::string &nickname) const {
  if (_bannedUsers.find(nickname) != _bannedUsers.end()) {
    return (true);
  }
  return (false);
}

void Channel::addBan(const std::string &nickname) {
  _bannedUsers.insert(nickname);
  std::ostringstream oss;
  oss << _nameWithPrefix << ": User " << nickname << " has been banned.";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}


/*============================================================================*/
/*       Modes handling                                                       */
/*============================================================================*/

/* invite-only (i) */
void Channel::activateInviteOnlyMode(void) {
  _mode.inviteOnly = true;

  std::ostringstream oss;
  oss << _nameWithPrefix << ": Invite-only mode activated";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

void Channel::deactivateInviteOnlyMode(void) {
  _mode.inviteOnly = false;

  std::ostringstream oss;
  oss << _nameWithPrefix << ": Invite-only mode desactivated";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

/* topic-settable-by-ops-only (t) */
void Channel::activateTopicOpsOnlyMode(void) {
  _mode.topicSettableByOpsOnly = true;

  std::ostringstream oss;
  oss << _nameWithPrefix << ": Topic settable by Operator only mode activated";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

void Channel::deactivateTopicOpsOnlyMode(void) {
  _mode.topicSettableByOpsOnly = false;

  std::ostringstream oss;
  oss << _nameWithPrefix
      << ": Topic settable by Operator only mode desactivated";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

/* key-mode (k) */
void Channel::updateKey(const std::string &newKey) {
  _key = newKey;

  std::ostringstream oss;
  oss << _nameWithPrefix << ": Channel key updated";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

void Channel::activateKeyMode(const std::string &key, const Client &client) {
  if (key.empty()) {
    send461NeedMoreParams(client, "MODE");
  } else {
    _mode.keyRequired = true;
    _key = key;

    std::ostringstream oss;
    oss << _nameWithPrefix << ": Key mode activated. Key set to " << key;
    Log::printLog(INFO_LOG, CHANNEL, oss.str());
  }
}

void Channel::deactivateKeyMode(void) {
  _mode.keyRequired = false;
  _key = "";

  std::ostringstream oss;
  oss << _nameWithPrefix << ": Key mode desactivated";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

/* operator (o) */
void Channel::addOperator(Client *client) {
  _channelOperators[client->getFd()] = client;

  std::ostringstream oss;
  oss << _nameWithPrefix << ": " << client->getNickname()
      << " has been promoted to operator";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

void Channel::removeOperator(Client *client) {
  _channelOperators.erase(client->getFd());

  std::ostringstream oss;
  oss << _nameWithPrefix << ": " << client->getNickname()
      << " has been demoted from operator";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

bool Channel::isOperator(int fd) const {
  if (_channelOperators.find(fd) != _channelOperators.end()) {
    return (true);
  }
  return (false);
}

/* limit-mode (l) */
void Channel::activateLimitMode(unsigned int limit) {
  if (limit == 0) {
    deactivateLimitMode();
  } else {
    _mode.limitSet = true;
    _limit = limit;

    std::ostringstream oss;
    oss << _nameWithPrefix << ": Limit mode activated. Limit set to " << _limit;
    Log::printLog(INFO_LOG, CHANNEL, oss.str());
  }
}

void Channel::deactivateLimitMode(void) {
  _mode.limitSet = false;
  _limit = 0;

  std::ostringstream oss;
  oss << _nameWithPrefix << ": Limit mode desactivated";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}

/*============================================================================*/
/*       Others                                                               */
/*============================================================================*/

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
