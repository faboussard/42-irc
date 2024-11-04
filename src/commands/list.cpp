/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:17:50 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/04 21:10:30 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

void parseArgument(const std::string &argument, \
                   std::vector<std::string> &channels);

void Server::list(const Client &client, const std::string &argument) {
  int fd = client.getFd();
  std::string nick = client.getNickname();
  send321Liststart(fd, nick);
  if (argument.empty()) {
    listAllChannels(fd, nick);
  } else { 
    std::vector<std::string> channels;
    parseArgument(argument, channels);
    listChannels(channels, client);
  }
  send323Listend(fd, nick);
}

void Server::listAllChannels(int fd, const std::string &nick) {
  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; it++)
    send322List(fd, nick, it->second);
}

void Server::listChannels(const std::vector<std::string> &channels, \
                          const Client &client) {
  int fd = client.getFd();
  std::string nick = client.getNickname();
  std::vector<std::string>::const_iterator itEnd = channels.end();
  for (std::vector<std::string>::const_iterator it = channels.begin(); it != itEnd; it++) {
    if ((*it)[0] == '#') {
      channelsMap::iterator itChannel = _channels.find(*it);
      if (itChannel != _channels.end())
        send322List(fd, nick, itChannel->second);
      else
        send403NoSuchChannel(client, *it);
    } else if ((*it)[0] == '&' || (*it)[0] == '!' || (*it)[0] == '+') {
      send476BadChanMask(client, *it);
    } else {
      send403NoSuchChannel(client, *it);
    }
  }
}

void parseArgument(const std::string &argument, \
                   std::vector<std::string> &channels) {
  std::stringstream ss(argument);
  std::string token;
  while (std::getline(ss, token, ',')) {
    channels.push_back(trimWhiteSpaces(token));
  }
}

// Following filters are not in RFC1459 standard 
// but can be implemented if we wish to do so

  // e.g. LIST >3
  // If argument is > followed by a number, 
  // list all channels with more than the given number of users

  // e.g. LIST C>60
  // If argument is C> followed by a number(n),
  // list all channels created at least n minutes ago

  // e.g. LIST T<60
  // If argument is T< followed by a number (n),
  // list all channels with a topic changed within the last n minutes