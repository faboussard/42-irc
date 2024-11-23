/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/23 15:50:21 by fanny            ###   ########.fr       */
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
#include "../../includes/Parser.hpp"

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
    oss << "client.getChannelsCount(): " << client.getChannelsCount();
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
  {
    std::ostringstream oss;
    oss << "gConfig->getLimitchanlimit: " << gConfig->getLimit(CHANLIMIT);
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
  KeyValuePairList channelsAndKeys = parseCommandIntoKeyValuePairList(channels, keys);
    if (channelsAndKeys.first.empty()) {
    send400UnknownError(client, channels, "channel name is empty");
    return;
  }
  for (size_t i = 0; i < channelsAndKeys.first.size(); ++i) {
#ifdef DEBUG
    {
        std::ostringstream oss;
        oss << "after parseJoinArguments: \n";
        printLog(DEBUG_LOG, COMMAND, oss.str());
      if (i < channelsAndKeys.first.size()) {
        std::ostringstream oss;
        oss << "channel: " << channelsAndKeys.first[i];
        printLog(DEBUG_LOG, COMMAND, oss.str());
      }
    }
    {
      if (i < channelsAndKeys.second.size()) {
        std::ostringstream oss;
        oss << "key: " << channelsAndKeys.second[i];
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
