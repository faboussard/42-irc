/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 14:48:41 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/13 16:49:06 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

// Commands
// INVITE clientA #42Channel ------> Invite clientA to #42Channel
// INVITE ------> Indicate a channel the client was invited to

void Server::invite(int invitingClientFd, const std::string &arg) {
  if (arg.empty()) {
    sendInvitList(invitingClientFd);
    return;
  }
  std::istringstream iss(arg);
  std::string nick, channelNameWithPrefix;
  iss >> nick >> channelNameWithPrefix;
  if (channelNameWithPrefix.empty()) {
    send461NeedMoreParams(_clients.at(invitingClientFd), "INVITE");
  } else if (!clientExists(nick)) {
    send401NoSuchNick(_clients.at(invitingClientFd), nick);
  } else if (channelNameWithPrefix[0] != '#') {
    send476BadChanMask(_clients.at(invitingClientFd), channelNameWithPrefix);
  } else if (!channelExists(channelNameWithPrefix)) {
    send403NoSuchChannel(_clients.at(invitingClientFd), channelNameWithPrefix);
  } else {
    Channel &channel = _channels.at(channelNameWithPrefix.substr(1));
    Client *invitedClient;
    clientsMap::const_iterator itEnd = _clients.end();
    for (clientsMap::iterator it = _clients.begin(); it != itEnd; ++it) {
      if (it->second.getNickname() == nick)
        invitedClient = &it->second;
    }
    if (!channel.isClientInChannel(invitingClientFd)) {
      send442NotOnChannel(_clients.at(invitingClientFd), channel);
    } else if (!channel.isOperator(invitingClientFd)) {
      send482ChanOPrivsNeeded(_clients.at(invitingClientFd), channel);
    } else if (channel.isClientInvited(invitedClient->getFd())) {
      send443UserOnChannel(_clients.at(invitingClientFd), nick, channel);
    } else {
      channel.addClientToInvitedMap(invitedClient);
      send341Inviting(_clients.at(invitingClientFd), nick, channel);
      invitedClient->receiveMessage(":" + \
      _clients.at(invitingClientFd).getNickname() + " INVITE " + nick + " :" \
      + channel.getNameWithPrefix() + "\r\n");
    }
  }
}

void Server::sendInvitList(int fd) const {
  const Client &client = _clients.at(fd);
  channelsMap::const_iterator itEnd = _channels.end();
  for (channelsMap::const_iterator it = _channels.begin(); it != itEnd; ++it) {
    if (it->second.isClientInChannel(fd)) {
      send336Invitelist(client, it->second);
    }
  }
  send337Endofinvitelist(client);
}

// ======= INVITE <nick of the person to be invited> <channel name> ============
// Replies:
// -- 341 RPL_INVITING:
// Confirms the user has been invited to the channel.

// -- 403 ERR_NOSUCHCHANNEL:
// The channel does not exist.

// -- 442 ERR_NOTONCHANNEL:
// Only members of the channel are allowed to invite other users.
// Otherwise, the server MUST reject the command with 442.

// -- 443 ERR_USERONCHANNEL:
// The user is already on the target channel, the server MUST reject with 443.

// -- 482 ERR_CHANOPRIVSNEEDED:
// The user is not a channel operator.

// ======== INVITE (without parameters) ========================================
// Replies:
// -- 336 RPL_INVITELIST:
// Indicate a channel the client was invited to.
// If the client is invited to multiple channels,
// the server sends multiple replies.
// If there is no channel to which the client was invited,
// the server sends only 337.

// -- 337 RPL_ENDOFINVITELIST:

// ======= INVITE (without not enough parameters) ==============================
// Replies:
// -- 461 ERR_NEEDMOREPARAMS:
