/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/06 19:18:30 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/serverConfig.hpp"
#include "../includes/utils.hpp"

void Server::joinChannel(const std::string &param, int fd) {
  // Check if the user is attempting to leave all channels (using "0" as param)
  if (isLeaveAllChannelsRequest(param)) {
    // partAllChannels(fd); // Leave all channels function
    return;
  }

  Client client = getClientByFd(fd);
  std::string::size_type spacePos = param.find(" ");
  std::string channelsPart = param.substr(0, spacePos);
  std::string keysPart =
      (spacePos != std::string::npos) ? param.substr(spacePos + 1) : "";
  stringVector channels;
  splitByComma(channelsPart, &channels);
  stringVector keys;
  splitByComma(keysPart, &keys);

    if (param.empty() || (param.length() == 1 && param[0] == REG_CHAN)) {
    send461NeedMoreParams(client, "JOIN");
    return;
    }

  for (size_t i = 0; i < channels.size(); ++i) {
    std::string channelName = channels[i];


  if (channelName.empty() || (channelName.length() == 1 && channelName[0] == REG_CHAN)) {
    send461NeedMoreParams(client, "JOIN");
    continue;
    }

    std::string channelNameWithoutPrefix = channelName.substr(1);

    if (!isChannelValid(channelName, client)) {
      continue;
    }

    if (_channels.find(channelNameWithoutPrefix) == _channels.end()) {
      createAndRegisterChannel(&client, channelNameWithoutPrefix);
    }

    std::string key = (i < keys.size()) ? keys[i] : "";
    Channel &channel = _channels[channelNameWithoutPrefix];

    if (channel.getMode().keyRequired && key != channel.getKey()) {
      send475BadChannelKey(client, channel);
      continue;
    }

    handleJoinRequest(fd, client, channelNameWithoutPrefix);
  }
}

bool Server::isValidChannelNameLength(const std::string &param) {
  return param.length() >= 1 && param.length() < 51;
}

bool Server::isValidChannelPrefix(const std::string &param) {
  return param[0] == REG_CHAN;
}

bool Server::isSingleCharacterChannelPrefix(const std::string &param) {
  return param.length() == 1 && param[0] == REG_CHAN;
}

bool Server::isLeaveAllChannelsRequest(const std::string &param) {
  return param == "0";
}

bool Server::isChannelValid(const std::string &param, const Client &client) {
  std::cout << " param Length: " << param.length() << std::endl;
   if (!isValidChannelNameLength(param) || !isValidChannelPrefix(param)) {
    send476BadChanMask(client, param);
    return false;
  } else if (static_cast<size_t>(client.getChannelsCount()) >=
             gConfig->getLimit("CHANLIMIT")) {
    send405TooManyChannels(client);
    return false;
  }
  return true;
}

void Server::createAndRegisterChannel(Client *client,
                                      const std::string &channelName) {
  addChanneltoServerIfNoExist(channelName);
  _channels[channelName].addClientToChannelMap(client);
  _channels[channelName].addOperatorsToChannelMap(client);
  client->incrementChannelsCount();
}

void Server::handleJoinRequest(int fd, const Client &client,
                               const std::string &channelName) {
  std::string nick = client.getNickname();
  sendJoinMessageToClient(fd, nick, channelName, client);
  send353Namreply(client, _channels[channelName]);
  std::cout << " _channels[channelName] " << _channels[channelName].getName()
            << std::endl;
  send366Endofnames(client, _channels[channelName]);
  broadcastJoinMessage(fd, nick, channelName);
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
  const std::map<int, Client *> &clientsInChannel =
      _channels[channelName].getChannelClients();
  std::map<int, Client *>::const_iterator it = clientsInChannel.begin();
  std::map<int, Client *>::const_iterator ite = clientsInChannel.end();

  for (; it != ite; ++it) {
    if (it->first != fd) {
      if (send(it->first, joinMessage.c_str(), joinMessage.length(), 0) == -1) {
        throw std::runtime_error("Runtime error: send failed");
      }
    }
  }
}
