/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/18 16:01:26 by faboussa         ###   ########.fr       */
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

void Server::joinChannel(int fd, const std::string &param) {
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
#ifdef DEBUG
  {
    std::ostringstream before;
    std::ostringstream after;
    before << "JOIN: Before split and trim channel: " CYAN << channelsPart
           << RESET;
    after << "JOIN: After split and trim channel: ";
    for (size_t i = 0; i < channels.size(); ++i)
      after << CYAN << channels[i] << RESET "|";
    printLog(DEBUG_LOG, COMMAND, before.str());
    printLog(DEBUG_LOG, COMMAND, after.str());
  }
#endif

  stringVector keys;
  std::string keysPart =
      (spacePos != std::string::npos) ? param.substr(spacePos + 1) : "";
  splitByCommaAndTrim(keysPart, &keys);
#ifdef DEBUG
  {
    std::ostringstream before;
    std::ostringstream after;
    before << "JOIN: Before split and trim key: " CYAN << keysPart << RESET;
    printLog(DEBUG_LOG, COMMAND, before.str());
    after << "JOIN: After split and trim key: ";
    for (size_t i = 0; i < keys.size(); ++i)
      after << CYAN << keys[i] << RESET "|";
    printLog(DEBUG_LOG, COMMAND, after.str());
  }
#endif
  for (size_t i = 0; i < channels.size(); ++i) {
    if (isChannelValid(channels[i], client)) {
      std::string channelNameWithoutPrefix = channels[i].substr(1);
      processJoinRequest(fd, &client, channelNameWithoutPrefix, keys, i);
    }
  }
}

bool Server::isChannelValid(const std::string &channelNameToCheck,
                            const Client &client) {
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "JOIN: Channel to check: " << channelToCheck;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "JOIN: client.getChannelsCount(): " << client.getChannelsCount();
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
  {
    std::ostringstream oss;
    oss << "JOIN: gConfig->getLimitchanlimit: " << gConfig->getLimit(CHANLIMIT);
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  if (channelNameToCheck.empty() ||
      (channelNameToCheck.length() == 1 && channelNameToCheck[0] == REG_CHAN)) {
    send461NeedMoreParams(client, "JOIN");
    return (false);
  } else if (channelNameToCheck.length() > gConfig->getLimit(CHANNELLEN) ||
             channelNameToCheck.length() < 2 ||
             channelNameToCheck[0] != REG_CHAN) {
    send476BadChanMask(client, channelNameToCheck.substr(1));
    return (false);
  }
  return (true);
}

void Server::processJoinRequest(int fd, Client *client,
                                const std::string &channelNameWithoutPrefix,
                                const stringVector &keys, size_t channelIndex) {
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "JOIN: Join request processing for channel " << REG_CHAN
        << channelNameWithoutPrefix;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  if (_channels.find(channelNameWithoutPrefix) == _channels.end()) {
    addChanneltoServer(channelNameWithoutPrefix);
    _channels.at(channelNameWithoutPrefix).addOperator(client);
  }
  Channel &channel = _channels.at(channelNameWithoutPrefix);
  bool keyHandled = false;
  if (channelIndex < keys.size() && !keys[channelIndex].empty()) {
    if (handleKey(client, channel, keys[channelIndex])) {
      keyHandled = true;
    }
  }
  // if bad key
  // send475BadChannelKey(client, channel);
  if (channel.getMode().limitSet &&
      channel.getLimit() <= channel.getChannelClients().size()) { // to be tested 
    send471ChannelIsFull(*client, channel);
    return;
  }
  if (channel.getMode().inviteOnly &&
      !channel.isClientInvited(client->getFd())) {
    send473InviteOnlyChan(*client, channel);
    return;
  }
    const clientPMap &clientsInChannel = channel.getChannelClients();
    if (clientsInChannel.find(fd) == clientsInChannel.end()) {
      channel.addClientToChannelMap(client);
      client->incrementChannelsCount();
      sendJoinMessageToClient(fd, client->getNickname(),
                              channelNameWithoutPrefix, *client);
      send353Namreply(*client, channel);
      send366Endofnames(*client, channel);
      broadcastInChannel(*client, channel, "JOIN", "say hello!");
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

void Server::addChanneltoServer(const std::string &channelName) {
  Channel newChannel(channelName);
  _channels[channelName] = newChannel;
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << newChannel.getNameWithPrefix() << ": New channel created";
    printLog(INFO_LOG, CHANNEL, oss.str());
  }
#endif
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
