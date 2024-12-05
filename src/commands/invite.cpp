/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 14:48:41 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/05 17:38:28 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <utility>

#include "../../includes/Server.hpp"

void Server::invite(int fd, const std::string &arg) {
  if (arg.empty()) {
    sendInvitedChannelsList(fd);
    return;
  }
  std::istringstream iss(arg);
  std::string invitedNick, channelName;
  iss >> invitedNick >> channelName;
  if (!inviteParamsValid(fd, invitedNick, channelName))
    return;
  else
    inviteClientToChannel(fd, invitedNick, channelName);
}

bool Server::inviteParamsValid(int invitingClientFd,
                               const std::string &invitedNick,
                               const std::string &channelName) {
  if (channelName.empty()) {
    send461NeedMoreParams(_clients.at(invitingClientFd), "INVITE");
    return (false);
  } else if (!nickExists(invitedNick)) {
    send401NoSuchNick(_clients.at(invitingClientFd), invitedNick);
    return (false);
  } else if (channelName[0] != REG_CHAN) {
    send476BadChanMask(_clients.at(invitingClientFd), channelName);
    return (false);
  } else if (!channelExists(channelName)) {
    send403NoSuchChannel(_clients.at(invitingClientFd), channelName);
    return (false);
  }
  return (true);
}

void Server::inviteClientToChannel(int invitingClientFd,
                                   const std::string &invitedNick,
                                   const std::string &channelName) {
  Channel &channel = _channels.at(channelName.substr(1));
  Client *invitedClient;
  clientsMap::const_iterator itEnd = _clients.end();
  for (clientsMap::iterator it = _clients.begin(); it != itEnd; ++it) {
    if (it->second.getNickname() == invitedNick) invitedClient = &it->second;
  }
  if (!channel.isClientInChannel(invitingClientFd)) {
    send442NotOnChannel(_clients.at(invitingClientFd), channel);
    return;
  } else if (!channel.isOperator(invitingClientFd)) {
    send482ChanOPrivsNeeded(_clients.at(invitingClientFd), channel);
    return;
  } else if (channel.isClientInChannel(invitedClient->getFd()) ||
             channel.isClientInvited(invitedClient->getFd())) {
    send443UserOnChannel(_clients.at(invitingClientFd), invitedNick, channel);
    return;
  } else {
    const std::string &invitingNick =
        _clients.at(invitingClientFd).getNickname();
    channel.addClientToInvitedMap(invitedClient, invitingNick);
    send341Inviting(_clients.at(invitingClientFd), invitedNick, channel);

    std::ostringstream oss;
    oss << ":" << invitingNick << " INVITE " << invitedNick << " :"
        << channel.getNameWithPrefix() << "\r\n";
    invitedClient->receiveMessage(oss.str());
  }
}

void Server::sendInvitedChannelsList(int fd) const {
  const Client &client = _clients.at(fd);
  channelsMap::const_iterator itEnd = _channels.end();
  for (channelsMap::const_iterator it = _channels.begin(); it != itEnd; ++it) {
    if (it->second.isClientInvited(fd)) {
      send336Invitelist(client, it->second);
    }
  }
  send337Endofinvitelist(client);
}
