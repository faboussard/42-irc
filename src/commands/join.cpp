/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/19 14:01:02 by faboussa         ###   ########.fr       */
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
  if (client.getChannelsCount() >= gConfig->getLimit(CHANLIMIT)) {
    send405TooManyChannels(client);
    return;
  }
  pairOfStringVectors channelsAndKeys = parseJoinArguments(param);
  for (size_t i = 0; i < channelsAndKeys.first.size(); ++i) {
#ifdef DEBUG
    {
      std::ostringstream oss;
      oss << "channel: " << channelsAndKeys.first[i];
      printLog(DEBUG_LOG, COMMAND, oss.str());
    }
    {
      std::ostringstream oss;
      oss << "key: " << channelsAndKeys.second[i];
      printLog(DEBUG_LOG, COMMAND, oss.str());
    }
#endif
    const std::string &channelName = channelsAndKeys.first[i];
    const std::string key =
        (i < channelsAndKeys.second.size()) ? channelsAndKeys.second[i] : "";
    if (isChannelNameValid(channelName, client)) {
      std::string channelNameWithoutPrefix = channelName.substr(1);
      channelsMap::iterator it = _channels.find(channelNameWithoutPrefix);
      if (it == _channels.end()) {
        addChanneltoServer(channelNameWithoutPrefix);
        it = _channels.find(channelNameWithoutPrefix);
        it->second.addOperator(&client);
      }
      Channel &channel = it->second;
      const Channel &constChannel = channel;
      if (isChannelNotFull(constChannel, client) &&
          isClientAllowedInInviteOnlyChannel(constChannel, client) &&
          isKeyValid(constChannel, key, client)) {
        processJoinRequest(fd, &client, &channel);
      }
    }
  }
}

pairOfStringVectors Server::parseJoinArguments(const std::string &param) {
  pairOfStringVectors channelsAndKeys;
  stringVector channels;
  std::string::size_type spacePos = param.find(" ");
  std::string channelsPart = param.substr(0, spacePos);
  splitByCommaAndTrim(channelsPart, &channels);
#ifdef DEBUG
  {
    std::ostringstream before, after;
    before << "CHANNELS: Before split and trim channel: " << channelsPart;
    after << "CHANNELS: After split and trim channel: ";
    for (size_t i = 0; i < channels.size(); ++i) after << channels[i] << "|";
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
    std::ostringstream before, after;
    before << "KEYS: Before split and trim key: " CYAN << keysPart << RESET;
    after << "KEYS: After split and trim key: ";
    for (size_t i = 0; i < keys.size(); ++i)
      after << CYAN << keys[i] << RESET "|";
    printLog(DEBUG_LOG, COMMAND, before.str());
    printLog(DEBUG_LOG, COMMAND, after.str());
  }
#endif
  channelsAndKeys.first = channels;
  channelsAndKeys.second = keys;
  return channelsAndKeys;
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

bool Server::isChannelNotFull(const Channel &channel, const Client &client) {
  if (channel.getMode().limitSet &&
      channel.getLimit() <= channel.getChannelClients().size()) {
    send471ChannelIsFull(client, channel);
    return false;
  }
  return true;
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
    oss << "Channel Name to check: " << channelNameToCheck;
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

void Server::processJoinRequest(int fd, Client *client, Channel *channel) {
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "Join request for channel: " << REG_CHAN << channel->getName();
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  const clientPMap &clientsInChannel = channel->getChannelClients();
  if (clientsInChannel.find(fd) == clientsInChannel.end()) {
    channel->addClientToChannelMap(client);
    client->incrementChannelsCount();
    sendJoinMessageToClient(fd, client->getNickname(), channel->getName(),
                            *client);
    send353Namreply(*client, *channel);
    send366Endofnames(*client, *channel);
    broadcastInChannel(*client, *channel, "JOIN", "say hello!");
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
  printLog(INFO_LOG, CHANNEL, oss.str());
}

void Server::sendJoinMessageToClient(int fd, const std::string &nick,
                                     const std::string &channelName,
                                     const Client &client) {
  std::string joinMessage = ":" + nick + " JOIN :#" + channelName + "\r\n";
  if (send(fd, joinMessage.c_str(), joinMessage.length(), 0) == -1) {
    printLog(ERROR_LOG, SYSTEM, "JOIN: send failed");
    throw std::runtime_error(RUNTIME_ERROR);
  }
  if (_channels[channelName].getTopic().topic.empty())
    send331Notopic(client, _channels[channelName]);
  else
    send332Topic(client, _channels[channelName]);
}
