/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:17:50 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/05 15:43:53 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

void parseArgument(const std::string &argument, stringVector *channels);

void Server::list(const Client &client, const std::string &argument) {
#ifdef TESTLIST
  Channel chan1("chan1");
  Channel chan2("chan2");
  _channels.insert(std::make_pair("chan1", chan1));
  _channels.insert(std::make_pair("chan2", chan2));
  _channels["chan1"].setTopic("This is a test channel for chan1", "AuthorName");
#endif
  int fd = client.getFd();
  std::string nick = client.getNickname();
  send321Liststart(fd, nick);
  if (argument.empty()) {
    listAllChannels(fd, nick);
  } else {
    stringVector channels;
    parseArgument(argument, &channels);
    listChannels(channels, client);
  }
  send323Listend(fd, nick);
}

void Server::listAllChannels(int fd, const std::string &nick) {
  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it)
    send322List(fd, nick, it->second);
}

void Server::listChannels(const stringVector &channels, const Client &client) {
  int fd = client.getFd();
  std::string nick = client.getNickname();
  stringVector::const_iterator itEnd = channels.end();
  for (stringVector::const_iterator it = channels.begin(); it != itEnd; ++it) {
    char prefix = (*it)[0];
    if (prefix == '#') {
      std::string toFind = std::string((*it).begin() + 1, (*it).end());
      channelsMap::iterator itChannel = _channels.find(toFind);
      if (itChannel != _channels.end())
        send322List(fd, nick, itChannel->second);
      else
        send403NoSuchChannel(client, *it);
    } else if (prefix == '&' || prefix == '!' || prefix == '+') {
        if (findChannel(*it))
          send476BadChanMask(client, *it);
        else
          send403NoSuchChannel(client, *it);  
    } else {
        send476BadChanMask(client, *it);
    }
  }
}

bool Server::findChannel(const std::string &channel) {
  std::string nameToFind = std::string(channel.begin() + 1, channel.end());
  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it) {
    if (it->first == nameToFind) return (true);
  }
  return (false);
}

void parseArgument(const std::string &argument, stringVector *channels) {
  std::stringstream ss(argument);
  std::string token;
  while (std::getline(ss, token, ',')) {
    channels->push_back(trimWhiteSpaces(token));
  }
}
