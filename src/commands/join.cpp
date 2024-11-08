/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/08 12:43:16 by faboussa         ###   ########.fr       */
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
  if (isLeaveAllChannelsRequest(param)) {
    quitChannel(fd);
    return;
  }

  Client &client = _clients.at(fd);
  stringVector channels;
  stringVector keys;
  parseJoinParams(param, &channels, &keys);

  if (param.empty() || (param.length() == 1 && param[0] == REG_CHAN)) {
    send461NeedMoreParams(client, "JOIN");
    return;
  }
  // pour chaque channel
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
    processJoinRequest(fd, &client, channelName, keys, i);
  }
}

void Server::parseJoinParams(const std::string &param, stringVector *channels,
                             stringVector *keys) {
  // comment enelver au prealable si on a  "#channel1,   ,#channel2 key1,key2"
  // =>ERROR
  //  ex : "#channel1,#channel2 key1,key2"; - trouve la
  //  position du space apres channel2.
  std::string::size_type spacePos = param.find(" ");
  std::string channelsPart = param.substr(0, spacePos);
  std::string keysPart =
      (spacePos != std::string::npos) ? param.substr(spacePos + 1) : "";
  splitByCommaAndTrim(channelsPart, channels);
  splitByCommaAndTrim(keysPart, keys);
}

// tester avec Client &client
void Server::processJoinRequest(int fd, Client *client, const std::string &channelName,
                                const std::vector<std::string> &keys,
                                size_t i) {
  std::string channelNameWithoutPrefix = channelName.substr(1);

#ifdef DEBUG
  std::cout << "ENTERED PROCESS JOIN REQUEST: " << channelNameWithoutPrefix
            << std::endl;
#endif

  // si le channel n'existe pas on l'ajoute
  if (_channels.find(channelNameWithoutPrefix) == _channels.end()) {
    addChanneltoServerIfNoExist(channelNameWithoutPrefix);
    _channels.at(channelNameWithoutPrefix).addOperator(client);
  }

  const clientPMap &clientsInChannel =
      _channels[channelNameWithoutPrefix].getChannelClients();
  Channel &channel = _channels.at(channelNameWithoutPrefix);

#ifdef DEBUG
  std::cout << "HERE 1: " << channelNameWithoutPrefix << std::endl;
#endif

  // si le client n'est pas déjà dans le channel
  if (clientsInChannel.find(fd) == clientsInChannel.end()) {
    client->incrementChannelsCount();
    channel.addClientToChannelMap(client);
  }

#ifdef DEBUG
  std::cout << "HERE 2: " << channelNameWithoutPrefix << std::endl;
#endif
  if (!keys.empty())
    handleKey(client, channel, keys[i]);
  sendJoinMessage(fd, *client, channelNameWithoutPrefix);
}

void Server::handleKey(Client *client, const Channel &channel, const std::string &key) {
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

void Server::sendJoinMessage(int fd, const Client &client,
                               const std::string &channelName) {

  // regarder ICI SEGFAULT ICI 
  std::string nick = client.getNickname();
  sendJoinMessageToClient(fd, nick, channelName, client);
  send353Namreply(client, _channels[channelName]);
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

  clientPMap clientsInChannel =
      getChannelByName(channelName).getChannelClients();

  clientPMap::iterator itEnd = clientsInChannel.end();
  clientPMap::iterator itBegin = clientsInChannel.begin();
  for (clientPMap::iterator it = itBegin; itBegin != itEnd; ++it) {
    if (it->first != fd) {  // Ne pas envoyer au client qui join
      if (send(it->first, joinMessage.c_str(), joinMessage.length(), 0) == -1) {
        throw std::runtime_error("Runtime error: send failed");
      }
    }
  }
}