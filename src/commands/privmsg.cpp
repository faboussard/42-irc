/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:18:52 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/05 16:43:21 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../includes/Server.hpp"

void Server::sendPrivmsgToClient(const Client &sender, const Client &receiver,
                                 const std::string &content) {
  std::string message = ":" + sender.getNickname() + " PRIVMSG " +
                        receiver.getNickname() + " " + content + "\r\n";
  receiver.receiveMessage(message);
}

bool Server::validPrivmsgTargets(const Client &client, stringVector *targets) {
  size_t TargetsSize = targets->size();
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "targetSize: " << TargetsSize;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
  {
    std::ostringstream oss;
    oss << "MAXTARGETS: " << gConfig->getLimit(MAXTARGETS);
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  if (TargetsSize > gConfig->getLimit(MAXTARGETS)) {
    send407TooManyTargets(client);
    return (false);
  }
  if (TargetsSize > 1) {
    size_t chanOpPrefixCount = 0;
    size_t channelsCount = 0;
    size_t clientsCount = 0;
    stringVector::iterator itEnd = (*targets).end();
    for (stringVector::iterator it = (*targets).begin(); it != itEnd; ++it) {
      std::string target = *it;
#ifdef DEBUG
      {
        std::ostringstream oss;
        oss << "target: " << target;
        printLog(DEBUG_LOG, COMMAND, oss.str());
      }
#endif
      if (target[0] == CHAN_OP)
        chanOpPrefixCount++;
      else if (target[0] == REG_CHAN)
        channelsCount++;
      else
        clientsCount++;
    }
    if (channelsCount != TargetsSize && clientsCount != TargetsSize &&
        chanOpPrefixCount != TargetsSize) {
      sendNotice(client,
                 "targets must be of same type (all clients, all channels or "
                 "all operators)");
      return (false);
    }
  }
  return (true);
}

bool Server::parsePrivmsgArguments(const std::string &arg, const Client &client,
                                   std::vector<std::string> *targetsVector,
                                   std::string *message) {
  if (arg.empty()) {
    send411NoRecipient(client, "PRIVMSG");
    return (false);
  }

  std::string::size_type spacePos = arg.find(" ");
  if (spacePos != std::string::npos) {
    *message = arg.substr(spacePos + 1);
  } else {
    *message = "";
  }
  if (message->empty()) {
    send412NoTextToSend(client);
    return (false);
  }
  std::string targetsPart = arg.substr(0, spacePos);
  splitByCommaAndTrim(targetsPart, targetsVector);
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "targetsPart: " << targetsPart;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
  {
    std::ostringstream oss;
    oss << "message: " << message;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  if (!validPrivmsgTargets(client, targetsVector)) {
    return (false);
  }
  return (true);
}

void Server::privmsg(int fd, const std::string &arg) {
  const Client &sender = _clients.at(fd);

  stringVector targetsVector;
  std::string message;
  if (parsePrivmsgArguments(arg, sender, &targetsVector, &message) == false) {
    return;
  }
  bool isChannel = (targetsVector[0][0] == REG_CHAN);
  bool isChanOpPrefix = (targetsVector[0][0] == CHAN_OP);
  stringVector::const_iterator itEnd = targetsVector.end();
  for (stringVector::const_iterator it = targetsVector.begin(); it != itEnd;
       ++it) {
    std::string target = *it;
    if (target[0] == CHAN_OP) target = target.substr(1);
#ifdef DEBUG
    {
      std::ostringstream oss;
      oss << "message: " << message;
      printLog(DEBUG_LOG, COMMAND, oss.str());
    }
#endif
    if (!channelExists(target) && !findClientByNickname(target)) {
      send401NoSuchNick(sender, target);
      return;
    }
    if (message[0] == ':') message = message.substr(1);
    if (channelExists(target)) {
      if (_channels.find(target.substr(1)) == _channels.end()) {
        return;
      }
      Channel &channel = _channels.at(target.substr(1));
      if (channel.getMode().inviteOnly &&
          !channel.isClientInvited(sender.getFd()) &&
          !channel.isClientInChannel(sender.getFd())) {
        send404CannotSendToChan(sender, channel);
        return;
      }
      if (isChanOpPrefix) {
#ifdef DEBUG
        {
          std::ostringstream oss;
          oss << "broadcast to operators only: " << target
              << " message: " << messageWithoutColon;
          printLog(DEBUG_LOG, COMMAND, oss.str());
        }
#endif
        broadcastToOperatorsOnly(sender, channel, "PRIVMSG", message);
      } else if (isChannel) {
#ifdef DEBUG
        {
          std::ostringstream oss;
          oss << "broadcast in channel: " << target
              << " message: " << messageWithoutColon;
          printLog(DEBUG_LOG, COMMAND, oss.str());
        }
#endif
        broadcastInChannelAndToSender(sender, channel, "PRIVMSG", message);
      }
    } else {
      Client *client = findClientByNickname(target);
      if (client) {
#ifdef DEBUG
        {
          std::ostringstream oss;
          oss << "send privmsg to client: " << target
              << " message: " << messageWithoutColon;
          printLog(DEBUG_LOG, COMMAND, oss.str());
        }
#endif
        sendPrivmsgToClient(sender, *client, message);
      }
    }
  }
}
