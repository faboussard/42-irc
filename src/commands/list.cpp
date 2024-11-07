/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:17:50 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/07 14:11:41 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"
#include "../../includes/utils.hpp"


void Server::list(const Client &client, const std::string &argument) {
#ifdef TESTLIST
  Channel chan1("chan1");
  Channel chan2("chan2");
  _channels.insert(std::make_pair("chan1", chan1));
  _channels.insert(std::make_pair("chan2", chan2));
  _channels["chan1"].setTopic("This is a test channel for chan1", "AuthorName");
  Client client1(4, "127.0.0.1", "localhost");
  client1.setNickname("client1");
  Client client2(5, "127.0.0.1", "localhost");
  client2.setNickname("client2");
  Client client3(6, "127.0.0.1", "localhost");
  client3.setNickname("client3");
  _channels["chan1"].acceptClientInTheChannel(&client1);
  _channels["chan1"].acceptClientInTheChannel(&client2);
  _channels["chan1"].acceptClientInTheChannel(&client3);
#endif
  int fd = client.getFd();
  std::string nick = client.getNickname();
  send321Liststart(fd, nick);
  if (argument.empty()) {
    listAllChannels(fd, nick);
  } else {
    stringVector channels;
    splitByCommaAndTrim(argument, &channels);
    listChannels(channels, client);
  }
  send323Listend(fd, nick);
}

void Server::listAllChannels(int fd, const std::string &nick) {
  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it)
  {
    std::cout << "Channel name: " << it->first << std::endl;
    send322List(fd, nick, it->second);
  }
}

void Server::listChannels(const stringVector &channels, const Client &client) {
  int fd = client.getFd();
  std::string nick = client.getNickname();
  stringVector::const_iterator itEnd = channels.end();
  for (stringVector::const_iterator it = channels.begin(); it != itEnd; ++it) {
    char prefix = (*it)[0];
    if (prefix == REG_CHAN) {
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

