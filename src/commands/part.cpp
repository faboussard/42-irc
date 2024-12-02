/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:53:20 by faboussa          #+#    #+#             */
/*   Updated: 2024/12/02 18:00:20 by fanny            ###   ########.fr       */
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

std::pair<std::vector<std::string>, std::string> Server::parsePartParams(const std::string &param) {
  std::vector<std::string> channelsVector;
  std::string reason;
  std::istringstream iss(param);
  std::string channels;
  iss >> channels;
  std::getline(iss >> std::ws, reason); // Read the rest of the line as the reason

  splitByCommaAndTrim(channels, &channelsVector);

#ifdef DEBUG
  {
    std::ostringstream before, afterKey, afterValue;
    before << "param: Before split and process: " << param;
    afterKey << "channelsVector: ";
    for (size_t i = 0; i < channelsVector.size(); ++i)
      afterKey << channelsVector[i] << "|";
    afterValue << "reason: " << reason;
    Server::printLog(DEBUG_LOG, COMMAND, before.str());
    Server::printLog(DEBUG_LOG, COMMAND, afterKey.str());
    Server::printLog(DEBUG_LOG, COMMAND, afterValue.str());
  }
#endif

  return std::make_pair(channelsVector, reason);
}

void Server::part(int fd, const std::string &param) {
  Client *client = &_clients.at(fd);
  if (param.empty()) {
    send461NeedMoreParams(*client, "PART");
    return;
  }

  
std::pair<std::vector<std::string>, std::string>  channelsAndReasons = parsePartParams(param);
  stringVector channelsVector = channelsAndReasons.first;
  std::string reason = channelsAndReasons.second;

  if (channelsVector.empty()) {
    send403NoSuchChannel(*client, param);
    return;
  }

  for (size_t i = 0; i < channelsVector.size(); ++i) {
    channelsMap::iterator it = _channels.find(channelsVector[i].substr(1));
    if (it == _channels.end()) {
      send403NoSuchChannel(*client, channelsVector[i]);
      continue;
    }
    Channel *channel = &it->second;
    if (channel->getChannelClients().find(fd) == channel->getChannelClients().end()) {
      send442NotOnChannel(*client, *channel);
      continue;
    }
    quitChannel(fd, channel, client, reason);
  }
}

void Server::quitChannel(int fd, Channel *channel, Client *client,
                         const std::string &reason) {
  broadcastInChannelAndToSender(*client, *channel, "PART", reason);

  client->decrementChannelsCount();
  channel->removeClientFromChannelMap(client);

  if (channel->getChannelOperators().find(fd) != channel->getChannelOperators().end()) {
    channel->removeOperator(client);
  }
}

void Server::quitAllChannels(int fd) {
  Client *client = &_clients.at(fd);
  std::vector<std::string> channelsToDelete;

  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it) {
    Channel *channel = &it->second;
    if (channel->getChannelClients().find(fd) !=
        channel->getChannelClients().end()) {
      quitChannel(fd, channel, client, "");
    }
  }
}
