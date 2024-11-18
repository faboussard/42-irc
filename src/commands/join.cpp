/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/18 09:35:59 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/colors.hpp"
#include "../../includes/numericReplies.hpp"
#include "../../includes/utils.hpp"

void Server::joinChannel(const std::string &param, int fd) {
  Client &client = _clients.at(fd);

  if (param.empty() || (param.length() == 1 && param[0] == REG_CHAN)) {
    send461NeedMoreParams(client, "JOIN");
    return;
  }
  if (isLeaveAllChannelsRequest(param)) {
    quitAllChannels(fd);
    return;
  }
  if (client.getChannelsCount() >= gConfig->getLimit(CHANLIMIT)) {
    send405TooManyChannels(client);
    return;
  }
  stringVector channels;
  std::string::size_type spacePos = param.find(" ");
  std::string channelsPart = param.substr(0, spacePos);
  splitByCommaAndTrim(channelsPart, &channels);
// #ifdef DEBUG
//   std::cout << "Before split and trim channel: " << channelsPart << std::endl;
//   std::cout << "After split and trim channel: ";
//   for (size_t i = 0; i < channels.size(); ++i) std::cout << channels[i] << "|";
//   std::cout << std::endl;
// #endif
#ifdef DEBUG
  std::ostringstream before;
  std::ostringstream after;
  before << "JOIN: Before split and trim channel: " CYAN
         << channelsPart << RESET;
  after << "JOIN: After split and trim channel: ";
  for (size_t i = 0; i < channels.size(); ++i)
    after << CYAN << channels[i] << RESET "|";
  printLog(DEBUG_LOG, COMMAND, before.str());
  printLog(DEBUG_LOG, COMMAND, after.str());
#endif

  stringVector keys;
  std::string keysPart =
      (spacePos != std::string::npos) ? param.substr(spacePos + 1) : "";
  splitByCommaAndTrim(keysPart, &keys);
// #ifdef DEBUG
//   std::cout << "Before split and trim key: " << keysPart << std::endl;
//   std::cout << "After split and trim key: ";
//   for (size_t i = 0; i < keys.size(); ++i) std::cout << keys[i] << "|";
//   std::cout << std::endl;
// #endif
#ifdef DEBUG
  before.clear();
  after.clear();
  before << "JOIN: Before split and trim key: " CYAN << keysPart << RESET;
  after << "JOIN: After split and trim key: ";
  for (size_t i = 0; i < keys.size(); ++i)
    after << CYAN << keys[i] << RESET "|";
  printLog(DEBUG_LOG, COMMAND, before.str());
  printLog(DEBUG_LOG, COMMAND, after.str());
#endif
  for (size_t i = 0; i < channels.size(); ++i) {
    if (isChannelValid(channels[i], client)) {
      std::string channelNameWithoutPrefix = channels[i].substr(1);
      processJoinRequest(fd, &client, channelNameWithoutPrefix, keys, i);
    }
  }
}

bool Server::isChannelValid(const std::string &channelToCheck,
                            const Client &client) {
#ifdef DEBUG
  // std::cout << std::endl << std::endl;
  // std::cout << "channel to check " << channelToCheck << std::endl;
  std::ostringstream oss;
  oss << "JOIN: Channel to check: " << channelToCheck;
  printLog(DEBUG_LOG, COMMAND, oss.str());
#endif
// #ifdef DEBUG
//     std::cout << std::endl << std::endl;
//     std::cout << "channelToCheck.length() " << channelToCheck.length() <<
//     std::endl; std::cout << " gConfig->getLimit(channelen)" <<
//     gConfig->getLimit(CHANNELLEN)  << std::endl;
// #endif
#ifdef DEBUG
  // std::cout << std::endl << std::endl;
  // std::cout << "client.getChannelsCount() " << client.getChannelsCount()
  //           << std::endl;
  // std::cout << "gConfig->getLimitchanlimit " << gConfig->getLimit(CHANLIMIT)
  //           << std::endl;
  oss.clear();
  oss << "JOIN: client.getChannelsCount() " << client.getChannelsCount();
  printLog(DEBUG_LOG, COMMAND, oss.str());
  oss.clear();
  oss << "JOIN: gConfig->getLimitchanlimit " << gConfig->getLimit(CHANLIMIT);
  printLog(DEBUG_LOG, COMMAND, oss.str());
#endif
  if (channelToCheck.empty() ||
      (channelToCheck.length() == 1 && channelToCheck[0] == REG_CHAN)) {
    send461NeedMoreParams(client, "JOIN");
    return (false);
  } else if (channelToCheck.length() > gConfig->getLimit(CHANNELLEN) ||
             channelToCheck.length() < 2 || channelToCheck[0] != REG_CHAN) {
    send476BadChanMask(client, channelToCheck.substr(1));
    return (false);
  } else if (client.getChannelsCount() >= gConfig->getLimit(CHANLIMIT)) {
    send405TooManyChannels(client);
    return (false);
  }
  return (true);
}

void Server::processJoinRequest(int fd, Client *client,
                                const std::string &channelNameWithoutPrefix,
                                const stringVector &keys, size_t channelIndex) {
  // si le channel n'existe pas on l'ajoute
#ifdef DEBUG
  // std::cout << "i am in process join request function " << std::endl;
  std::ostringstream oss;
  oss << "JOIN: Join request processing for channel " << channelName;
  printLog(DEBUG_LOG, COMMAND, oss.str());
#endif
  if (_channels.find(channelNameWithoutPrefix) == _channels.end()) {
    addChanneltoServerIfNoExist(channelNameWithoutPrefix);
    _channels.at(channelNameWithoutPrefix).addOperator(client);
  }

  Channel &channel = _channels.at(channelNameWithoutPrefix);
  bool keyHandled = false;
  if (channelIndex < keys.size() && !keys[channelIndex].empty()) {
    if (handleKey(client, channel, keys[channelIndex])) {
      keyHandled = true;
    }
  }
  if (keyHandled || keys.empty()) {
    const clientPMap &clientsInChannel =
        _channels[channelNameWithoutPrefix].getChannelClients();
    Channel &channel = _channels.at(channelNameWithoutPrefix);
    // si le client n'est pas déjà dans le channel
    std::string nick = client->getNickname();
    if (clientsInChannel.find(fd) == clientsInChannel.end()) {
      broadcastInChannel(*client, channel, "JOIN", "say hello!");
      client->incrementChannelsCount();
      channel.addClientToChannelMap(client);
      sendJoinMessageToClient(fd, nick, channelNameWithoutPrefix, *client);
      send353Namreply(*client, channel);
      send366Endofnames(*client, channel);
      // broadcastJoinMessage(fd, nick, channelNameWithoutPrefix);
    }
  }
}

bool Server::handleKey(Client *client, const Channel &channel,
                       const std::string &key) {
  if (channel.getMode().keyRequired && key != channel.getKey()) {
    send475BadChannelKey(*client, channel);
    return (false);
  }
  return (true);
}

bool Server::isLeaveAllChannelsRequest(const std::string &param) {
  return (param == "0");
}

void Server::addChanneltoServerIfNoExist(const std::string &channelName) {
  if (_channels.find(channelName) == _channels.end()) {
    Channel newChannel(channelName);
    _channels[channelName] = newChannel;

    std::ostringstream oss;
    oss << newChannel.getNameWithPrefix() << ": New channel created";
    printLog(INFO_LOG, CHANNEL, oss.str());
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

// void Server::broadcastJoinMessage(int fd, const std::string &nick,
//                                   const std::string &channelName) {
//   std::string joinMessage = ":" + nick + " JOIN :#" + channelName + "\r\n";

//   clientPMap clientsInChannel =
//       findChannelByName(channelName).getChannelClients();
//   clientPMap::iterator itEnd = clientsInChannel.end();
//   clientPMap::iterator it = clientsInChannel.begin();
//   for (; it != itEnd; ++it) {
//     if (it->first != fd) {  // Ne pas envoyer au client qui join
//       if (send(it->first, joinMessage.c_str(), joinMessage.length(), 0) ==
//       -1) {
//         throw std::runtime_error("Runtime error: send failed");
//       }
//     }
//   }
// }
