/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 14:48:41 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/14 08:28:28 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

// Commands
// INVITE clientNick #42Channel ------> Invite clientNick to #42Channel
// INVITE ------> Indicate a channel the client was invited to

void Server::invite(int invitingClientFd, const std::string &arg) {
  if (arg.empty()) {
    sendInvitedChannelsList(invitingClientFd);
    return;
  }
  std::istringstream iss(arg);
  std::string nick, channelNameWithPrefix;
  iss >> nick >> channelNameWithPrefix;
  if (!inviteParamsValid(invitingClientFd, nick, channelNameWithPrefix))
    return;
  else
    inviteClientToChannel(invitingClientFd, nick, channelNameWithPrefix);
}

bool Server::inviteParamsValid(int invitingClientFd,
                               const std::string &invitedNick,
                               const std::string &channelNameWithPrefix) {
  if (channelNameWithPrefix.empty()) {
    send461NeedMoreParams(_clients.at(invitingClientFd), "INVITE");
    return (false);
  } else if (!nickExists(invitedNick)) {
    send401NoSuchNick(_clients.at(invitingClientFd), invitedNick);
    return (false);
  } else if (channelNameWithPrefix[0] != REG_CHAN) {
    send476BadChanMask(_clients.at(invitingClientFd), channelNameWithPrefix);
    return (false);
  } else if (!channelExists(channelNameWithPrefix)) {
    send403NoSuchChannel(_clients.at(invitingClientFd), channelNameWithPrefix);
    return (false);
  }
  return (true);
}

void Server::inviteClientToChannel(int invitingClientFd,
                                   const std::string &invitedNick,
                                   const std::string &channelNameWithPrefix) {
  Channel &channel = _channels.at(channelNameWithPrefix.substr(1));
  Client *invitedClient;
  clientsMap::const_iterator itEnd = _clients.end();
  for (clientsMap::iterator it = _clients.begin(); it != itEnd; ++it) {
    if (it->second.getNickname() == invitedNick)
      invitedClient = &it->second;
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
    channel.addClientToInvitedMap(invitedClient);
    send341Inviting(_clients.at(invitingClientFd), invitedNick, channel);
    invitedClient->receiveMessage(
        ":" + _clients.at(invitingClientFd).getNickname() + " INVITE " +
        invitedNick + " :" + channel.getNameWithPrefix() + "\r\n");
  }
}

/// send to invited client list
void Server::sendInvitedChannelsList(int fd) const {
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
