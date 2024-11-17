/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:20:03 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/17 21:37:04 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <string>

#include "../../includes/Server.hpp"

// kick #channelName #targetNick #reason (reason is optional)

void Server::kick(int fd, const std::string &param) {
  Client &client = _clients.at(fd);
  if (param.empty()) {
    send461NeedMoreParams(client, "KICK");
    return;
  }
  std::istringstream iss(param);
  std::string channelName, targetNick, reason;
  std::getline(iss, channelName, ' ');
  std::getline(iss, targetNick, ' ');
  if (targetNick.empty()) {
    send461NeedMoreParams(client, "KICK");
    return;
  }
  // Lire la raison si elle existe
  if (iss.rdbuf()->in_avail() > 0) {
    std::getline(iss >> std::ws, reason);
  }
  if (reason.length() >= gConfig->getLimit(KICKLEN)) {
    reason = reason.substr(0, gConfig->getLimit(KICKLEN));
  }
  if (reason[0] == ':') {
    reason = reason.substr(1);
  }
#ifdef DEBUG
  std::cout << "[KICK] Channel: " << channelName << " / Target: " << targetNick
            << " / Reason: " << reason << std::endl;
#endif
  if (!channelExists(channelName)) {
    send403NoSuchChannel(client, channelName);
    return;
  }
  Channel &channel = _channels.at(channelName.substr(1));
  if (!channel.isOperator(client.getFd())) {
    send482ChanOPrivsNeeded(client, channel);
    return;
  }
  Client *targetClient = findClientByNickname(targetNick);
  if (targetClient == NULL) {
    send401NoSuchNick(client, targetNick);
    return;
  }
  int fdTarget = targetClient->getFd();
  if (!channel.isClientInChannel(fdTarget)) {
    send441UserNotInChannel(client, targetNick, channel);
    return;
  }
  std::string message = targetNick + " " + reason + "\r\n";
  broadcastInChannel(client, channel, "KICK", message);
  quitChannel(fdTarget, &channel, targetClient);
}

/*
KICK message
      Command: KICK
   Parameters: <channel> <user> *( "," <user> ) [<comment>]
The KICK command can be used to request the forced removal of a user from a
channel. It causes the <user> to be removed from the <channel> by force.

This message may be sent from a server to a client to notify the client that
someone has been removed from a channel. In this case, the message <source> will
be the client who sent the kick, and <channel> will be the channel which the
target client has been removed from.

If no comment is given, the server SHOULD use a default message instead.

Servers MUST NOT send multiple users in this message to clients, and MUST
distribute these multiple-user KICK messages as a series of messages with a
single user name on each. This is necessary to maintain backward compatibility
with existing client software. If a KICK message is distributed in this way,
<comment> (if it exists) should be on each of these messages.

Servers MAY limit the number of target users per KICK command via the TARGMAX
parameter of RPL_ISUPPORT, and silently drop targets if the number of targets
exceeds the limit.

Numeric Replies:

ERR_NEEDMOREPARAMS (461) OK
ERR_NOSUCHCHANNEL (403)  OK
ERR_CHANOPRIVSNEEDED (482) OK
ERR_USERNOTINCHANNEL (441) OK
ERR_NOTONCHANNEL (442) OK
Deprecated Numeric Reply:

ERR_BADCHANMASK (476) deprecated
Examples:

   KICK #Finnish Matthew           ; Command to kick Matthew from
                                   #Finnish

   KICK &Melbourne Matthew         ; Command to kick Matthew from
                                   &Melbourne

   KICK #Finnish John :Speaking English
                                   ; Command to kick John from #Finnish
                                   using "Speaking English" as the
                                   reason (comment).

   :WiZ!jto@tolsun.oulu.fi KICK #Finnish John
                                   ; KICK message on channel #Finnish
                                   from WiZ to remove John from channel

ERR_USERNOTINCHANNEL (441)
  "<client> <nick> <channel> :They aren't on that channel"
Returned when a client tries to perform a channel+nick affecting command, when
the nick isn’t joined to the channel (for example, MODE #channel +o nick).


KICKLEN Parameter
  Format: KICKLEN=<length>
The KICKLEN parameter indicates the maximum length for the <reason> of a KICK
command. If a KICK <reason> has more characters than this parameter, it may be
silently truncated by the server before being passed on to other clients.
Clients MAY receive a KICK <reason> that has more characters than this
parameter.

The value MUST be specified and MUST be a positive integer.

Examples:

  KICKLEN=255

  KICKLEN=307*/
