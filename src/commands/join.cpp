/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/28 18:32:27 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../includes/Client.hpp"
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
  std::string channels, keys;
  std::istringstream iss(param);
  iss >> channels >> keys;
  StringVectorPair channelsAndKeys = parsejoin(channels, keys);
  if (channelsAndKeys.first.empty()) {
    send400UnknownError(client, channels, "channel name is empty");
    return;
  }
  for (size_t i = 0; i < channelsAndKeys.first.size(); ++i) {
#ifdef DEBUG
    {
      std::ostringstream oss;
      oss << "JOIN: after parseJoinArguments: \n";
      printLog(DEBUG_LOG, COMMAND, oss.str());
      if (i < channelsAndKeys.first.size()) {
        std::ostringstream oss;
        oss << "JOIN: channel: " << channelsAndKeys.first[i];
        printLog(DEBUG_LOG, COMMAND, oss.str());
      }
    }
    {
      if (i < channelsAndKeys.second.size()) {
        std::ostringstream oss;
        oss << "JOIN: key: " << channelsAndKeys.second[i];
        printLog(DEBUG_LOG, COMMAND, oss.str());
      }
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

StringVectorPair Server::parsejoin(const std::string &key,
                                   const std::string &value) {
  stringVector keyVector;
  stringVector valueVector;
  StringVectorPair list;
  splitByCommaAndTrim(key, &keyVector);

#ifdef DEBUG
  {
    std::ostringstream before, after;
    before << "JOIN: key: Before split and trim key: " << key;
    after << "JOIN: keyVector: After split and trim keyVector: ";
    for (size_t i = 0; i < keyVector.size(); ++i) after << keyVector[i] << "|";
    Server::printLog(DEBUG_LOG, COMMAND, before.str());
    Server::printLog(DEBUG_LOG, COMMAND, after.str());
  }
#endif
  if (keyVector.size() != static_cast<std::vector<std::string>::size_type>(
                              std::count(key.begin(), key.end(), ',') + 1)) {
    return StringVectorPair();
  }
  splitByCommaAndTrim(value, &valueVector);
#ifdef DEBUG
  {
    std::ostringstream before, after;
    before << "JOIN: value: Before split and trim value: " << value;
    after << "JOIN: valueVector: After split and trim valueVector: ";
    for (size_t i = 0; i < valueVector.size(); ++i)
      after << valueVector[i] << "|";
    Server::printLog(DEBUG_LOG, COMMAND, before.str());
    Server::printLog(DEBUG_LOG, COMMAND, after.str());
  }
#endif
  list.first = keyVector;
  list.second = valueVector;
  return (list);
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
    oss << "JOIN: Channel Name to check: " << channelNameToCheck;
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
    oss << "JOIN: Join request for channel: " << REG_CHAN << channel->getName();
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  const clientPMap &clientsInChannel = channel->getChannelClients();
  if (clientsInChannel.find(fd) == clientsInChannel.end()) {
    channel->addClientToChannelMap(client);
    client->incrementChannelsCount();
    sendJoinMessageToClient(client->getNickname(), channel->getName(), *client);
    send353Namreply(*client, *channel);
    send366Endofnames(*client, *channel);
    broadcastInChannelAndToSender(*client, *channel, "JOIN", "say hello!");
  }
  if (channel->getTopic().topic.empty())
    send331Notopic(*client, *channel);
  else
    send332Topic(*client, *channel);
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

void Server::sendJoinMessageToClient(const std::string &nick,
                                     const std::string &channelName,
                                     const Client &client) {
  std::string joinMessage = ":" + nick + " JOIN :#" + channelName + "\r\n";
  client.receiveMessage(joinMessage);
}
