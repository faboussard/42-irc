/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/05 17:14:34 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"
#include "../includes/serverConfig.hpp"


void Server::joinChannels(const std::string &param, int fd, Client &client) {
  if (isJoinZero(param)) {
    #ifdef TEST
      std::cout << "client: " << fd << RED" has parted all channels " << std::endl;
    #endif
    // Appel à une fonction pour quitter tous les canaux
    //partAllChannels(fd, client); // fonction qui gère le PART
    return;
  }

  std::string::size_type start = 0;
  std::string::size_type pos = param.find(",");

  while (pos != std::string::npos) {
    std::string channelName = param.substr(start, pos - start);

    if (isChannelValid(channelName, fd, client)) {
      #ifdef TEST
        std::cout << "client: " << fd << " joins channel " << channelName << std::endl;
      #endif
      executeJoin(fd, client, channelName);  // Exécution de la commande JOIN
    }

    start = pos + 1;
    pos = param.find(",", start);
  }

  // Gérer le dernier canal en dehors de la boucle
  std::string lastChannel = param.substr(start);
  if (isChannelValid(lastChannel, fd, client)) {
    #ifdef TEST
      std::cout << "client: " << fd << " joins last channel " << lastChannel << std::endl;
    #endif
    executeJoin(fd, client, lastChannel);
  }
}

bool Server::isValidLength(const std::string &param) {
  return param.length() < 51;
}

bool Server::hasNoSpaces(const std::string &param) {
  return param.find(" ") == std::string::npos;
}

bool Server::isJoinZero(const std::string &param) { return param == "0"; }

bool Server::goodChannelName(const std::string &param) {
  return isValidLength(param) && hasNoSpaces(param);
}

bool Server::isChannelValid(const std::string &param, int fd, Client &client) {
 if (param.empty()) {
#ifdef TEST
    std::cout << "client: " << fd << RED" has no channel name" RESET<< std::endl;
    return false;
#endif
    send461NeedMoreParams(client, "JOIN");
  }
  } else if (!goodChannelName(param)) {
#ifdef TEST
    std::cout << "client: " << fd << RED" has a bad channel name"RESET << std::endl;
    return false;
#endif
    send476BadChanMask(client, param);
  } else if (client.getChannelsCount() >= gConfig->getLimit("CHANLIMIT"))
  {
#ifdef TEST
    std::cout << "client: " << fd << RED " has too many channels" RESET << std::endl;
    return false;
#endif
    send405TooManyChannels(client);
  }
  return true;
}

void Server::executeJoin(int fd, Client &client,
                         const std::string &channelName) {
  addChanneltoServer(channelName);
  _channels[channelName].addClientToChannelMap(client);
  client.incrementChannelsCount();
#ifdef TEST
  std::cout << "client: " << fd << " has joined the Channel "
            << _channels[channelName].getName() << std::endl;
  std::cout << std::endl;
  return;
#endif
  std::string nick = client.getNickname();
  sendJoinMessageToClient(fd, nick, channelName);
  send353Namreply(fd, nick, _channels[channelName]);
  std::cout << " _channels[channelName] " << _channels[channelName].getName()
            << std::endl;
  send366Endofnames(client, channelName);
  broadcastJoinMessage(fd, nick, channelName);
}

void Server::addChanneltoServer(const std::string &channelName) {
  if (_channels.find(channelName) == _channels.end()) {
    Channel newChannel(channelName);
    _channels[channelName] = newChannel;
  }
}

void Server::sendJoinMessageToClient(int fd, const std::string &nick,
                                     const std::string &channelName) {
  std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
  if (send(fd, joinMessage.c_str(), joinMessage.length(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
  if (_channels[channelName].getTopic().topic.empty())
    send331Notopic(fd, nick, _channels[channelName]);
  else
    send332Topic(fd, nick, _channels[channelName]);
}

void Server::broadcastJoinMessage(int fd, const std::string &nick,
                                  const std::string &channelName) {
  std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
  const clientsMap &clientsInChannel =
      _channels[channelName].getClientsInChannel();
  clientsMap::const_iterator it = clientsInChannel.begin();
  clientsMap::const_iterator ite = clientsInChannel.end();

  for (it = clientsInChannel.begin(); it != ite; ++it) {
    if (it->first != fd) {
      if (send(it->first, joinMessage.c_str(), joinMessage.length(), 0) == -1)
        throw std::runtime_error(RUNTIME_ERROR);
    }
  }
#ifdef DEBUG
  std::cout << GREY "broadcastJoinMessage ok " << nick
            << " joined channel " RESET << channelName << std::endl;
#endif
}
