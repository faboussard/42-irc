/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/08 15:40:18 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"

void Server::joinChannel(const std::string &param, int fd) {
  if (param.find_first_of(" \t\n\r\f\v") != std::string::npos) {
    std::cout << RED
        "ERROR FROM EDITORS : channel name cannot or key contain "
        "whitespaces" RESET
              << std::endl;
    return;
  }
  if (isLeaveAllChannelsRequest(param)) {
    quitChannel(fd);
    return;
  }
  Client &client = _clients.at(fd);
  if (param.empty() || (param.length() == 1 && param[0] == REG_CHAN)) {
    send461NeedMoreParams(client, "JOIN");
    return;
  }
  std::cout << "HERE 1" << std::endl;
  stringVector channels;
  std::string::size_type spacePos = param.find(" ");
  std::string channelsPart = param.substr(0, spacePos);

  std::cout << "HERE 2" << channelsPart << std::endl;

  splitByCommaAndTrim(channelsPart, &channels);
  checkChannelsCorrectness(channels);

  if (channels.empty()) {
    send461NeedMoreParams(client, "JOIN");
    return;
  }
  std::cout << "HERE 3" << channelsPart << std::endl;

  for (size_t i = 0; i < channels.size(); ++i) {
    std::string channelName = channels[i];

    if (channelName.empty() ||
        (channelName.length() == 1 && channelName[0] == REG_CHAN)) {
      send461NeedMoreParams(client, "JOIN");
      continue;
    }
    if (!isChannelValid(channelName, client)) {
      continue;
    }
    stringVector keys;
    std::string keysPart =
        (spacePos != std::string::npos) ? param.substr(spacePos + 1) : "";
    splitByCommaAndTrim(keysPart, &keys);
    checkKeysCorrectness(keys);
    std::cout << "HERE 4" << channelsPart << std::endl;
    if (!keys.empty()) {
      send525InvalidKey(client, getChannelByName(channelName));
      return;
    }
    processJoinRequest(fd, &client, channelName, keys, i);
  }
}

void Server::checkChannelsCorrectness(const stringVector &channels) {
  stringVector::const_iterator it = channels.begin();
  stringVector::const_iterator itEnd = channels.end();
  for (; it != itEnd; ++it) {
    if (it->empty() || it->find_first_of(" \t\n\r\f\v") != std::string::npos) {
      std::cout << RED
          "ERROR FROM EDITORS : channel name cannot contain whitespaces or be "
          "empty" RESET
                << std::endl;
      return;
    }
  }
}

void Server::checkKeysCorrectness(const stringVector &keys) {
  stringVector::const_iterator it = keys.begin();
  stringVector::const_iterator itEnd = keys.end();
  for (; it != itEnd; ++it) {
    if (it->find_first_of(" \t\n\r\f\v") != std::string::npos) {
      std::cout << RED
          "ERROR FROM EDITORS : channel name cannot contain whitespaces or be "
          "empty" RESET
                << std::endl;
      return;
    }
  }
}

// tester avec Client &client
void Server::processJoinRequest(int fd, Client *client,
                                const std::string &channelName,
                                const std::vector<std::string> &keys,
                                size_t i) {
  std::string channelNameWithoutPrefix = channelName.substr(1);

  // si le channel n'existe pas on l'ajoute
  if (_channels.find(channelNameWithoutPrefix) == _channels.end()) {
    addChanneltoServerIfNoExist(channelNameWithoutPrefix);
    _channels.at(channelNameWithoutPrefix).addOperator(client);
  }

  const clientPMap &clientsInChannel =
      _channels[channelNameWithoutPrefix].getChannelClients();
  Channel &channel = _channels.at(channelNameWithoutPrefix);
  if (!keys.empty()) handleKey(client, channel, keys[i]);

  // si le client n'est pas déjà dans le channel
  std::string nick = client->getNickname();

  if (clientsInChannel.find(fd) == clientsInChannel.end()) {
    client->incrementChannelsCount();
    channel.addClientToChannelMap(client);
    sendJoinMessageToClient(fd, nick, channelName, *client);
    send353Namreply(*client, _channels[channelName]);
    send366Endofnames(*client, _channels[channelName]);
  }
  broadcastJoinMessage(fd, nick, channelName);
}

void Server::handleKey(Client *client, const Channel &channel,
                       const std::string &key) {
  if (channel.getMode().keyRequired && key != channel.getKey()) {
    send475BadChannelKey(*client, channel);
    return;
  }
}

bool Server::isValidChannelNameLength(const std::string &param) {
  return param.length() >= 1 && param.length() < 51;
}

bool Server::isValidChannelPrefix(const std::string &param) {
  return param[0] == REG_CHAN;
}

bool Server::isLeaveAllChannelsRequest(const std::string &param) {
  return param == "0";
}

bool Server::isChannelValid(const std::string &param, const Client &client) {
  if (!isValidChannelNameLength(param) || !isValidChannelPrefix(param)) {
    send476BadChanMask(client, param);
    return false;
  } else if (client.getChannelsCount() >= gConfig->getLimit("CHANLIMIT")) {
    send405TooManyChannels(client);
    return false;
  }
  return true;
}

void Server::addChanneltoServerIfNoExist(const std::string &channelName) {
  if (_channels.find(channelName) == _channels.end()) {
    Channel newChannel(channelName);
    _channels[channelName] = newChannel;
  }
}

void Server::sendJoinMessageToClient(int fd, const std::string &nick,
                                     const std::string &channelName,
                                     const Client &client) {
  std::string joinMessage = ":" + nick + " JOIN :#" + channelName + "\r\n";
  if (send(fd, joinMessage.c_str(), joinMessage.length(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
  if (_channels[channelName].getTopic().topic.empty())
    send331Notopic(client, _channels[channelName]);
  else
    send332Topic(client, _channels[channelName]);
}

void Server::broadcastJoinMessage(int fd, const std::string &nick,
                                  const std::string &channelName) {
  std::string joinMessage = ":" + nick + " JOIN :#" + channelName + "\r\n";

  clientPMap clientsInChannel =
      getChannelByName(channelName).getChannelClients();
  clientPMap::iterator itEnd = clientsInChannel.end();
  clientPMap::iterator it = clientsInChannel.begin();
  for (; it != itEnd; ++it) {
    if (it->first != fd) {  // Ne pas envoyer au client qui join
      if (send(it->first, joinMessage.c_str(), joinMessage.length(), 0) == -1) {
        throw std::runtime_error("Runtime error: send failed");
      }
    }
  }
}