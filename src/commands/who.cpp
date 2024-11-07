/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 11:04:35 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/07 16:43:30 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"

// Supports only 2 patterns:
// WHO
// WHO <#channel>

// After JOIN #chanName --> Hexchat sends MODE #chanName & WHO #chanName

void Server::who(const Client &client, const std::string &arg) {
  if (arg.empty()) {
#ifdef DEBUG
    std::cout << "Listing all clients for each Channel..." << std::endl;
#endif
    channelsMap::iterator itEnd = _channels.end();
    for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it) {
#ifdef DEBUG
      std::cout << "Listing all clients for " << it->first << "..."
                << std::endl;
#endif
      sendClientsListInChannel(client, it->second);
    }
  } else if (arg[0] != '#') {
    send476BadChanMask(client, arg);
  } else {
    if (findChannel(arg)) {
#ifdef DEBUG
      std::cout << "Listing all clients for " << arg << "..." << std::endl;
#endif
      std::string::const_iterator it = arg.begin();
      const Channel &channel = getChannelByName(std::string(it + 1, arg.end()));
      sendClientsListInChannel(client, channel);
    } else {
      send403NoSuchChannel(client, arg);
    }
  }
}

void Server::sendClientsListInChannel(const Client &client,
                                      const Channel &channel) {
  const clientPMap &clientsInChannel = channel.getChannelClients();
  clientPMap::const_iterator itEnd = clientsInChannel.end();
#ifdef DEBUG
   std::cout << "Number of clients in " << channel.getName() << ": "
			 << clientsInChannel.size() << std::endl;
#endif
  for (clientPMap::const_iterator it = clientsInChannel.begin(); it != itEnd;
       ++it) {
#ifdef DEBUG
    std::cout << "Sending WHO reply for " << it->second->getNickname() << "..."
              << std::endl;
#endif
    send352Whoreply(client, *it->second, channel);
  }
  send315EndOfWho(client, channel);
}
