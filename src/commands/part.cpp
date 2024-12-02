/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:53:20 by faboussa          #+#    #+#             */
/*   Updated: 2024/12/02 15:53:24 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <map>
#include <stdexcept>
#include <string>

#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/colors.hpp"
#include "../../includes/numericReplies.hpp"
#include "../../includes/utils.hpp"

stringVectorPair Server::parsePartParams(const std::string &param,
                                         Client *client) {
  stringVector channelsVector;
  stringVector reasonsVector;
  stringVectorPair list;
  stringVector tokens = basicSplit(param);
  for (size_t i = 0; i < tokens.size(); ++i) {
    if (!tokens[i].empty() && tokens[i][0] == '#') {
      channelsVector.push_back(tokens[i]);
    } else {
      reasonsVector.push_back(tokens[i]);
    }
  }
#ifdef DEBUG
  {
    std::ostringstream before, afterKey, afterValue;
    before << "param: Before split and process: " << param;
    afterKey << "channelsVector: ";
    for (size_t i = 0; i < channelsVector.size(); ++i)
      afterKey << channelsVector[i] << "|";
    afterValue << "reasonsVector: ";
    for (size_t i = 0; i < reasonsVector.size(); ++i)
      afterValue << reasonsVector[i] << "|";
    Server::printLog(DEBUG_LOG, COMMAND, before.str());
    Server::printLog(DEBUG_LOG, COMMAND, afterKey.str());
    Server::printLog(DEBUG_LOG, COMMAND, afterValue.str());
  }
#endif
  list.first = channelsVector;
  list.second = reasonsVector;
  return list;
}

void Server::part(int fd, const std::string &param) {
  Client *client = &_clients.at(fd);
  if (param.empty()) {
    send461NeedMoreParams(*client, "PART");
    return;
  }
  stringVectorPair channelsAndReasons = parsePartParams(param, client);
  stringVector channelsVector = channelsAndReasons.first;
  stringVector reasonsVector = channelsAndReasons.second;
  if (channelsVector.empty()) {
    send403NoSuchChannel(*client, param);
    return;
  }
  for (size_t i = 0; i < channelsVector.size(); ++i) {
    Channel *channel = &_channels[channelsVector[i].substr(1)];
    if (!channel) {
      send403NoSuchChannel(*client, channelsVector[i]);
      return;
    }
    if (channel->getChannelClients().find(fd) ==
        channel->getChannelClients().end()) {
      send442NotOnChannel(*client, *channel);
      return;
    }
    std::string reason = reasonsVector.size() > i ? reasonsVector[i] : "";
    quitChannel(fd, channel, client, reason);
  }
}

void Server::quitChannel(int fd, Channel *channel, Client *client,
                         const std::string &reason) {
  sendPartMessageToClient(fd, client->getNickname(), channel->getName());
  client->decrementChannelsCount();
  channel->removeClientFromChannelMap(client);
  if (channel->getChannelOperators().find(fd) !=
      channel->getChannelOperators().end()) {
    channel->removeOperator(client);
  }
  if (!reason.empty()) {
    broadcastInChannelAndToSender(*client, *channel, "PART", reason);
  }
}

void Server::quitAllChannels(int fd) {
  Client *client = &_clients.at(fd);
  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it) {
    Channel *channel = &it->second;
    if (channel->getChannelClients().find(fd) !=
        channel->getChannelClients().end()) {
      quitChannel(fd, channel, client);
    }
  }
}

void Server::sendPartMessageToClient(int fd, const std::string &nick,
                                     const std::string &channelName) {
  std::string partMessage = ":" + nick + " PART :#" + channelName + "\r\n";
  if (send(fd, partMessage.c_str(), partMessage.length(), 0) == -1) {
    throw std::runtime_error("Runtime error: send failed");
  }
}
