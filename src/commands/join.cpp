/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/12/09 10:44:13 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../includes/Client.hpp"
#include "../../includes/Log.hpp"
#include "../../includes/Parser.hpp"
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
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "JOIN: client.getChannelsCount(): " << client.getChannelsCount();
    Log::printLog(DEBUG_LOG, COMMAND, oss.str());
  }
  {
    std::ostringstream oss;
    oss << "JOIN: gConfig->getLimitchanlimit: " << gConfig->getLimit(CHANLIMIT);
    Log::printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  stringVectorPair channelsAndKeys = parseJoinParams(param);
  for (size_t i = 0; i < channelsAndKeys.first.size(); ++i) {
#ifdef DEBUG
    {
      std::ostringstream oss;
      oss << "JOIN: after parseJoinArguments: \n";
      Log::printLog(DEBUG_LOG, COMMAND, oss.str());
      if (i < channelsAndKeys.first.size()) {
        std::ostringstream oss;
        oss << "JOIN: channel: " << channelsAndKeys.first[i];
        Log::printLog(DEBUG_LOG, COMMAND, oss.str());
      }
    }
    {
      if (i < channelsAndKeys.second.size()) {
        std::ostringstream oss;
        oss << "JOIN: key: " << channelsAndKeys.second[i];
        Log::printLog(DEBUG_LOG, COMMAND, oss.str());
      }
    }
#endif
    const std::string &channelName = channelsAndKeys.first[i];
    const std::string key =
        (i < channelsAndKeys.second.size()) ? channelsAndKeys.second[i] : "";
    if (isChannelNameValid(channelName, client)) {
      std::string channelNameWithoutPrefix = channelName.substr(1);
      channelsMap::iterator it = _channels.begin();
      channelsMap::iterator itEnd = _channels.end();
      for (; it != itEnd; ++it) {
        std::string currentChannelName = it->first;
        std::string requestedChannelName = channelNameWithoutPrefix;
        strToUpper(&currentChannelName);
        strToUpper(&requestedChannelName);
        if (currentChannelName == requestedChannelName) break;
      }
      if (it == _channels.end()) {
        addChanneltoServer(channelNameWithoutPrefix);
        it = _channels.find(channelNameWithoutPrefix);
        it->second.addOperator(&client);
      }
      Channel &channel = it->second;
      const Channel &constChannel = channel;
      std::map<int, Client *> clientsInChannel = channel.getChannelClients();
      if (client.getChannelsCount() >= gConfig->getLimit(CHANLIMIT) &&
          clientsInChannel.find(fd) == clientsInChannel.end()) {
        send405TooManyChannels(client);
        return;
      }
      if (!isChannelFull(constChannel, client) &&
          isClientAllowedInInviteOnlyChannel(constChannel, client) &&
          isKeyValid(constChannel, key, client)) {
        processJoinRequest(fd, &client, &channel);
      }
    }
  }
}

stringVectorPair Server::parseJoinParams(const std::string &param) {
  std::string channels, keys;
  std::istringstream iss(param);
  iss >> channels >> keys;
  stringVector channelsVector;
  stringVector keysVector;
  stringVectorPair list;
  splitByCommaAndTrim(channels, &channelsVector);
#ifdef DEBUG
  {
    std::ostringstream before, after;
    before << "JOIN: channels: Before split and trim key: " << param;
    after << "JOIN: channelsVector: After split and trim channelsVector: ";
    for (size_t i = 0; i < channelsVector.size(); ++i)
      after << channelsVector[i] << "|";
    Log::printLog(DEBUG_LOG, COMMAND, before.str());
    Log::printLog(DEBUG_LOG, COMMAND, after.str());
  }
#endif
  splitByCommaAndTrim(keys, &keysVector);
#ifdef DEBUG
  {
    std::ostringstream before, after;
    before << "JOIN: keys: Before split and trim value: " << param;
    after << "JOIN: keysVector: After split and trim keysVector: ";
    for (size_t i = 0; i < keysVector.size(); ++i)
      after << keysVector[i] << "|";
    Log::printLog(DEBUG_LOG, COMMAND, before.str());
    Log::printLog(DEBUG_LOG, COMMAND, after.str());
  }
#endif
  return (make_pair(channelsVector, keysVector));
}

bool Server::isKeyValid(const Channel &channel, const std::string &key,
                        const Client &client) {
  if (channel.getMode().keyRequired && !channel.getKey().empty() &&
      channel.getKey() != key) {
    send475BadChannelKey(client, channel);
    return (false);
  }
  return (true);
}

bool Server::isChannelFull(const Channel &channel, const Client &client) {
  if (channel.getMode().limitSet &&
      channel.getLimit() <= channel.getChannelClients().size() &&
      !channel.isClientInChannel(client.getFd())) {
    send471ChannelIsFull(client, channel);
    return true;
  }
  return false;
}

bool Server::isClientAllowedInInviteOnlyChannel(const Channel &channel,
                                                const Client &client) {
  if (channel.getMode().inviteOnly &&
      !channel.isClientInvited(client.getFd())) {
    send473InviteOnlyChan(client, channel);
    return false;
  }
  return true;
}

bool Server::isChannelNameValid(const std::string &channelNameToCheck,
                                const Client &client) {
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "JOIN: Channel Name to check: " << channelNameToCheck;
    Log::printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  if (channelNameToCheck.empty() || channelNameToCheck.length() < 2) {
    send461NeedMoreParams(client, "JOIN");
    return (false);
  }
  if (channelNameToCheck[0] != REG_CHAN) {
    send476BadChanMask(client, channelNameToCheck);
    return (false);
  }
  if (channelNameToCheck.length() > gConfig->getLimit(CHANNELLEN)) {
    send476BadChanMask(client, channelNameToCheck);
    return (false);
  }
  return (true);
}

void Server::processJoinRequest(int fd, Client *client, Channel *channel) {
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "JOIN: Join request for channel: " << channel->getName();
    Log::printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  const clientPMap &clientsInChannel = channel->getChannelClients();
  if (clientsInChannel.find(fd) == clientsInChannel.end()) {
    channel->addClientToChannelMap(client);
    client->incrementChannelsCount();
    broadcastInChannelAndToSenderNoContent(*client, *channel, "JOIN");
    send353Namreply(*client, *channel);
    send366Endofnames(*client, *channel);
    if (!channel->getTopic().topic.empty()) send332Topic(*client, *channel);
  }
}

bool Server::isLeaveAllChannelsRequest(const std::string &param) {
  return (param == "0");
}

void Server::addChanneltoServer(const std::string &channelName) {
  Channel newChannel(channelName);
  _channels[channelName] = newChannel;
  std::ostringstream oss;
  oss << newChannel.getNameWithPrefix() << ": New channel created";
  Log::printLog(INFO_LOG, CHANNEL, oss.str());
}
