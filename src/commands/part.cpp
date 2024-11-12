/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:53:20 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/12 11:53:30 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


// ajout fonction is client in channel

#include <map>
#include <stdexcept>
#include <string>

#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/colors.hpp"
#include "../../includes/numericReplies.hpp"
#include "../../includes/utils.hpp"

void Server::quitAllChannels(int fd) {
  Client *client = &_clients.at(fd);

  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it) {
    Channel *channel = &it->second;
    if (channel->getChannelClients().find(fd) !=
        channel->getChannelClients().end()) {
      sendPartMessageToClient(fd, client->getNickname(), channel->getName());
      broadcastPartMessage(fd, client->getNickname(), channel->getName());
      client->decrementChannelsCount();
      channel->removeClientFromChannelMap(client);
    }
    if (channel->getChannelOperators().find(fd) !=
        channel->getChannelOperators().end()) {
      channel->removeOperator(client);
    }
  }
}

void Server::broadcastPartMessage(int fd, const std::string &nick,
                                  const std::string &channelName) {
  std::string partMessage = ":" + nick + " PART :#" + channelName + "\r\n";

  clientPMap clientsInChannel =
      getChannelByName(channelName).getChannelClients();

  for (clientPMap::iterator it = clientsInChannel.begin();
       it != clientsInChannel.end(); ++it) {
    if (it->first != fd) {
      if (send(it->first, partMessage.c_str(), partMessage.length(), 0) == -1) {
        throw std::runtime_error("Runtime error: send failed");
      }
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
