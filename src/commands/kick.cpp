/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:20:03 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/09 10:44:43 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>

#include <iostream>
#include <sstream>
#include <string>

#include "../../includes/Log.hpp"
#include "../../includes/Server.hpp"

void Server::parseKickParams(const std::string &param, const Client &client,
                             std::string *channelName, std::string *targetNick,
                             std::string *reason) {
  std::istringstream iss(param);

  std::getline(iss, *channelName, ' ');
  std::getline(iss, *targetNick, ' ');
  if (targetNick->empty()) {
    send461NeedMoreParams(client, "KICK");
    return;
  }
  if (iss.rdbuf()->in_avail() > 0) {
    std::getline(iss >> std::ws, *reason);
  }
  if (reason->length() >= gConfig->getLimit(KICKLEN)) {
    *reason = reason->substr(0, gConfig->getLimit(KICKLEN));
  }
  if ((*reason)[0] == ':') {
    *reason = reason->substr(1);
  }
}

void Server::kick(int fd, const std::string &param) {
  Client &client = _clients.at(fd);
  if (param.empty()) {
    send461NeedMoreParams(client, "KICK");
    return;
  }
  std::string channelName, targetNick, reason;
  parseKickParams(param, client, &channelName, &targetNick, &reason);
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "Channel: " << channelName << " / Target: " << targetNick
        << " / Reason: " << reason;
    Log::printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  if (!channelExists(channelName)) {
    send403NoSuchChannel(client, channelName);
    return;
  }
  Channel &channel = _channels.at(channelName.substr(1));
  if (!channel.isOperator(client.getFd())) {
    send482ChanOPrivsNeeded(client, channel);
    return;
  }
  Client *targetClient = findClientByNickname(targetNick);
  if (targetClient == NULL) {
    send401NoSuchNick(client, targetNick);
    return;
  }
  int fdTarget = targetClient->getFd();
  if (!channel.isClientInChannel(fdTarget)) {
    send441UserNotInChannel(client, targetNick, channel);
    return;
  }
  if (&client == targetClient) {
    send400UnknownError(client, "KICK", "cannot kick yourself");
    return;
  }
  kickUser(client, &channel, targetClient, reason);
}

void Server::kickUser(const Client &client, Channel *channel,
                      Client *targetClient, const std::string &reason) {
  std::string message = targetClient->getNickname() + " " + reason + "\r\n";
  kickBroadcast(client, *channel, "KICK", message);
  targetClient->decrementChannelsCount();
  channel->removeClientFromChannelMap(targetClient);
  if (channel->getChannelOperators().find(targetClient->getFd()) !=
      channel->getChannelOperators().end()) {
    channel->removeOperator(targetClient);
  }
}

void Server::kickBroadcast(const Client &sender, const Channel &channel,
                           const std::string &command,
                           const std::string &content) {
  std::string message = ":" + sender.getNickname() + " " + command + " " +
                        channel.getNameWithPrefix() + " " + content + "\r\n";
  const clientPMap &allClients = channel.getChannelClients();
  clientPMap::const_iterator itEnd = allClients.end();
  for (clientPMap::const_iterator it = allClients.begin(); it != itEnd; ++it) {
    Client *client = it->second;
    client->receiveMessage(message);
  }
}
