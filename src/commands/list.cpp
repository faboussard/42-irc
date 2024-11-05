/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:17:50 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/05 10:12:08 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

// LIST [<channel>{,<channel>}]

void parseArgument(const std::string &argument, stringVector *channels);

void Server::list(const Client &client, const std::string &argument) {
  std::cout << "Argument |" << argument << "|" << std::endl;
  #ifdef TESTLIST
    Channel chan1 = Channel("chan1");
    Channel chan2 = Channel("chan2");
    _channels["chan1"] = chan1;
    _channels["chan2"] = chan2;
    chan1.setTopic("This is a test channel for chan1", "AuthorName");
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
    if ((*it)[0] == '#') {
      channelsMap::iterator itChannel = _channels.find(*it);
      if (itChannel != _channels.end())
        send322List(fd, nick, itChannel->second);
      else
        send403NoSuchChannel(client, *it);
    } else if (((*it)[0] == '&' || (*it)[0] == '!' || (*it)[0] == '+') && \
               findChannel(*it)) {
      send476BadChanMask(client, *it);
    } else {
      send403NoSuchChannel(client, *it);
    }
  }
}

bool Server::findChannel(const std::string &channel) {
  std::string::const_iterator nameStart = channel.begin() + 1;
  std::string nameToFind = std::string(nameStart, channel.end());
  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it) {
    if (it->first == nameToFind)
      return (true);
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

// Wildcard Matching
// * : Matches zero or more characters in a channel name.
// ? : Matches exactly one character in a channel name.

// enum FilterType {
//   FULLNAME,    // #
//   WILDCARD_A,  // # + *
//   WILDCARD_Q,  // # + ?
//   BADMASK,     // & or ! or +
//   INVALID
// };
