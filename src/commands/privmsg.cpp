/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:18:52 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/18 15:30:30 by faboussa         ###   ########.fr       */
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

bool Server::validPrivmsgTargets(const std::string &arg, const Client &client,
                                 stringVector *targets) {
  size_t commaCount = std::count(arg.begin(), arg.end(), ',');
#ifdef DEBUG
{
  std::ostringstream oss;
  oss << "PRIVMSG: commaCount: " << commaCount;
  printLog(DEBUG_LOG, COMMAND, oss.str());
}
{
  std::ostringstream oss;
  oss << "PRIVMSG: MAXTARGETS: " << gConfig->getLimit(MAXTARGETS);
  printLog(DEBUG_LOG, COMMAND, oss.str());
}
#endif
  if (commaCount + 1 > gConfig->getLimit(MAXTARGETS)) {
    send407TooManyTargets(client);
    return (false);
  }
  if (commaCount != 0) {
    size_t chanOpPrefixCount = 0;
    size_t channelsCount = 0;
    size_t clientsCount = 0;

    stringVector::iterator itEnd = (*targets).end();
    for (stringVector::iterator it = (*targets).begin(); it != itEnd; ++it) {
      std::string target = *it;
#ifdef DEBUG
      {
        std::ostringstream oss;
        oss << "PRIVMSG: target: " << target;
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
    if (commaCount != 0 && channelsCount != commaCount + 1 &&
        clientsCount != commaCount + 1 && chanOpPrefixCount != commaCount + 1) {
      send407TooManyTargets(client);
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
  if (message->empty() || (*message)[0] != ':') {
    send412NoTextToSend(client);
    return (false);
  }
  std::string targetsPart = arg.substr(0, spacePos);
  splitByCommaAndTrim(targetsPart, targetsVector);

#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "PRIVMSG: targetsPart: " << targetsPart;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
  {
    std::ostringstream oss;
    oss << "PRIVMSG: " CYAN "in parsearg: " RESET << message;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  if (!validPrivmsgTargets(arg, client, targetsVector)) {
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
#ifdef DEBUG
  // std::cout << "[message]  " << message << std::endl;
  {
    std::ostringstream oss;
    oss << "PRIVMSG: " CYAN "message: " RESET << message;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif

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
    oss << "PRIVMSG: TRIMMED target: " << message;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
    if (!channelExists(target) && !findClientByNickname(target)) {
      send401NoSuchNick(sender, target);
      return;
    }
    std::string messageWithoutColon = message.substr(1);

    if (channelExists(target)) {
      if (_channels.find(target.substr(1)) == _channels.end()) {
        return;
      }
      Channel &channel = _channels.at(target.substr(1));
      if (channel.getMode().inviteOnly &&
          !channel.isClientInvited(sender.getFd())) {
        send404CannotSendToChan(sender, channel);
        return;
      }
      if (isChanOpPrefix) {
        broadcastToOperatorsOnly(sender, channel, "PRIVMSG",
                                 messageWithoutColon);
      } else if (isChannel) {
        broadcastInChannel(sender, channel, "PRIVMSG", messageWithoutColon);
      }
    } else {
      Client *client = findClientByNickname(target);
      if (client) {
        sendPrivmsgToClient(sender, *client, messageWithoutColon);
      }
    }
  }
}
