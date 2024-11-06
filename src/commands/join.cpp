/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/06 15:38:21 by faboussa         ###   ########.fr       */
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
  if (isJoinZero(param)) {
#ifdef TEST
    std::cout << "client: " << fd << RED " has parted all channels "
              << std::endl;
#endif
    // TODO: Appel à une fonction pour quitter tous les canaux
    // TODO: partAllChannels(fd, client); // fonction qui gère le PART
    return;
  }
  Client client = getClientByFd(fd);
  std::string::size_type start = 0;
  std::string::size_type pos = param.find(",");
  int count = 0;
std::string ChannelNameWithoutPrefix;
  while (pos != std::string::npos) {
    count++;
    // on va jusqua la virgule et on gere le nom de channel
    if ((pos == ',' || count == 1)) {
      if (isChannelValid(param, client)) {
#ifdef DEBUG
        std::cout << "client: " << fd << " joins channel "
                  << ChannelNameWithoutPrefix << std::endl;
#endif
    std::string channelName = param.substr(start, pos - start);
    ChannelNameWithoutPrefix = channelName.substr(1);
        registerChannel(&client, ChannelNameWithoutPrefix);
      }
    }
    // on gere les keys qui sont apres les espaces 
    else if (isValidLength(param)) {
      std::string key = param.substr(0, 1); 
#ifdef DEBUG
      std::cout << "client: " << fd << " has a key " << key << std::endl;
#endif
      if (_channels[ChannelNameWithoutPrefix].getMode().keyRequired)
        if (key != _channels[ChannelNameWithoutPrefix].getKey())
          send475BadChannelKey(client, _channels[ChannelNameWithoutPrefix]);
      return;
    }
    executeJoin(fd, client, ChannelNameWithoutPrefix);
    start = pos + 1;
    pos = param.find(",", start);
  }

  std::string lastChannel = param.substr(start);
  std::string ChannelNameWithoutPrefix = lastChannel.substr(1);
  if (isChannelValid(ChannelNameWithoutPrefix, client)) {
#ifdef DEBUG
    std::cout << "client: " << fd << " joins last channel "
              << ChannelNameWithoutPrefix << std::endl;
#endif
    executeJoin(fd, client, ChannelNameWithoutPrefix);
  }
}

bool Server::isValidLength(const std::string &param) {
  return param.length() >= 1 && param.length() < 51;
}

bool Server::isValidPrefix(const std::string &param) {
  return param[0] == REG_CHAN;
}

bool Server::isJoinZero(const std::string &param) { return param == "0"; }

bool Server::isChannelValid(const std::string &param, const Client &client) {
  if (param.empty()) {
#ifdef TEST
    std::cout << "client: " << fd << RED " has no channel name" RESET
              << std::endl;
    return false;
#endif
    send461NeedMoreParams(client, "JOIN");
    return false;
  } else if (!isValidLength(param) && !isValidPrefix(param)) {
#ifdef TEST
    std::cout << "client: " << fd << RED " has a bad channel name" RESET
              << std::endl;
    return false;
#endif
    send476BadChanMask(client, param);
    return false;
  } else if (static_cast<size_t>(client.getChannelsCount()) >=
             gConfig->getLimit("CHANLIMIT")) {
#ifdef TEST
    std::cout << "client: " << fd << RED " has too many channels" RESET
              << std::endl;
    return false;
#endif
    send405TooManyChannels(client);
    return false;
  }
  return true;
}

void Server::registerChannel(Client *client,
                             const std::string &channelName) {
  addChanneltoServer(channelName);
  _channels[channelName].addClientToChannelMap(client);
  _channels[channelName].addOperatorsToChannelMap(client);
  client->incrementChannelsCount();
}

void Server::executeJoin(int fd, const Client &client,
                         const std::string &channelName) {
#ifdef TEST
  std::cout << "client: " << fd << " has joined the Channel "
            << _channels[channelName].getName() << std::endl;
  std::cout << std::endl;
  return;
#endif
  std::string nick = client.getNickname();
  sendJoinMessageToClient(fd, nick, channelName, client);
  send353Namreply(client, _channels[channelName]);
  std::cout << " _channels[channelName] " << _channels[channelName].getName()
            << std::endl;
  send366Endofnames(client, _channels[channelName]);
  broadcastJoinMessage(fd, nick, channelName);
}

void Server::addChanneltoServer(const std::string &channelName) {
  if (_channels.find(channelName) == _channels.end()) {
    Channel newChannel(channelName);
    _channels[channelName] = newChannel;
  }
}

void Server::sendJoinMessageToClient(int fd, const std::string &nick,
                                     const std::string &channelName,
                                     const Client &client) {
  std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
  if (send(fd, joinMessage.c_str(), joinMessage.length(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
  if (_channels[channelName].getTopic().topic.empty())
    send331Notopic(client, _channels[channelName]);
  else
    send332Topic(client, _channels[channelName]);
}

void Server::broadcastJoinMessage(int fd, const std::string &nick,
                                  const std::string &channelName) {
  std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
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
#ifdef DEBUG
  std::cout << "broadcastJoinMessage: " << joinMessage << std::endl;
#endif
}
