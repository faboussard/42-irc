/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/08 17:28:48 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"

void Server::joinChannel(const std::string &param, int fd) {
  Client &client = _clients.at(fd);

  if (param.empty() || (param.length() == 1 && param[0] == REG_CHAN)) {
    send461NeedMoreParams(client, "JOIN");
    return;
  }
  if (isLeaveAllChannelsRequest(param)) {
    quitChannel(fd);
    return;
  }
  stringVector channels;
  std::string::size_type spacePos = param.find(" ");
  std::string channelsPart = param.substr(0, spacePos);
  splitByCommaAndTrim(channelsPart, &channels);
  if (isChannelsCorrect(channels, client)) {
    for (size_t i = 0; i < channels.size(); ++i) {
      std::string channelNameWithoutPrefix = channels[i].substr(1);

      stringVector keys;
      std::string keysPart =
          (spacePos != std::string::npos) ? param.substr(spacePos + 1) : "";
      splitByCommaAndTrim(keysPart, &keys);
      checkKeysCorrectness(keys);
      if (!keys.empty()) {
        send525InvalidKey(client, getChannelByName(channelNameWithoutPrefix));
        return;
      }
      processJoinRequest(fd, &client, channelNameWithoutPrefix, keys, i);
    }
  }
}

bool Server::isChannelsCorrect(const stringVector &channels,
                               const Client &client) {
  stringVector::const_iterator it = channels.begin();
  stringVector::const_iterator itEnd = channels.end();
  for (; it != itEnd; ++it) {
    if (it->empty() || (it->length() == 1 && (*it)[0] == REG_CHAN)) {
      send461NeedMoreParams(client, "JOIN");
      return false;
    }
    if (it->find_first_of(" \t\n\r\f\v,") != std::string::npos) {
      std::cout << RED
          "ERROR : channel name cannot contain whitespaces or commas or be "
          "empty" RESET
                << std::endl;
      return false;
    }
    if (it->length() > 20 || it->length() < 2 || (*it)[0] != REG_CHAN) {
      send476BadChanMask(client, it->substr(1));
      return false;
    } else if (client.getChannelsCount() >= gConfig->getLimit("CHANLIMIT")) {
      send405TooManyChannels(client);
      return false;
    }
  }
  return true;
}

void Server::checkKeysCorrectness(const stringVector &keys) {
  stringVector::const_iterator it = keys.begin();
  stringVector::const_iterator itEnd = keys.end();
  for (; it != itEnd; ++it) {
    if (it->find_first_of(" \t\n\r\f\v,") != std::string::npos) {
      std::cout << RED
          "ERROR : keys cannot contain whitespaces or commas or be empty" RESET
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
  // si le channel n'existe pas on l'ajoute
  if (_channels.find(channelName) == _channels.end()) {
    addChanneltoServerIfNoExist(channelName);
    _channels.at(channelName).addOperator(client);
  }

  const clientPMap &clientsInChannel =
      _channels[channelName].getChannelClients();
  Channel &channel = _channels.at(channelName);
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

bool Server::isLeaveAllChannelsRequest(const std::string &param) {
  return param == "0";
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
