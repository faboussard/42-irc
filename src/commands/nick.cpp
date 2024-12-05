/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/12/05 23:05:03 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "../../includes/Log.hpp"
#include "../../includes/Parser.hpp"
#include "../../includes/colors.hpp"

static bool nickAlreadyUsed(clientsMap::iterator it, const std::string &nick);
static void broadcastNickChange(const std::string &oldNick,
                                const Client &client,
                                const channelsMap &chanMap);

bool Parser::verifyNick(const std::string &nick, Client *client,
                        clientsMap *cltMap, const channelsMap &chanMap,
                        bool alreadyRegisterd) {
  if (nick.empty()) {
    send431NoNicknameGiven(*client);
    return (false);
  }
  size_t size = nick.size();
  if (size > gConfig->getLimit(NICKLEN) || std::isdigit(nick[0])) {
    send432ErroneusNickname(*client, nick);
    return (false);
  }
  for (size_t i = 0; i < size; ++i) {
    if (!std::isalnum(nick[i]) && nick[i] != '[' && nick[i] != ']' &&
        nick[i] != '{' && nick[i] != '}' && nick[i] != '\\' && nick[i] != '|' &&
        nick[i] != '`' && nick[i] != '_' && nick[i] != '^' && nick[i] != '-') {
      send432ErroneusNickname(*client, nick);
      return (false);
    }
  }
  clientsMap::iterator itEnd = cltMap->end();
  for (clientsMap::iterator it = cltMap->begin(); it != itEnd; ++it) {
    if (nickAlreadyUsed(it, nick)) {
      if (it->second.getFd() != client->getFd())
        send433NickAlreadyInUse(*client, nick);
      return (false);
    }
  }
  std::string oldNick;
  if (alreadyRegisterd) oldNick = client->getNickname();
  client->setNickname(nick);
  if (alreadyRegisterd) broadcastNickChange(oldNick, *client, chanMap);
  {
    std::ostringstream oss;
    oss << client->getNickname() << " (fd" << client->getFd()
        << "): Nickname validated";
    Log::printLog(INFO_LOG, CLIENT, oss.str());
  }
  return (true);
}

bool nickAlreadyUsed(clientsMap::iterator it, const std::string &nick) {
  std::string nickToCompare = it->second.getNickname();
  strToUpper(&nickToCompare);
  std::string requestedNick = nick;
  strToUpper(&requestedNick);
#ifdef DEBUG
  std::ostringstream oss;
  oss << "Comparing " << nickToCompare << " with " << requestedNick;
  Log::printLog(DEBUG_LOG, CLIENT, oss.str());
#endif
  if (nickToCompare == requestedNick) {
    return (true);
  }
  return (false);
}

// should not send more than once to each client
void broadcastNickChange(const std::string &oldNick, const Client &client,
                         const channelsMap &chanMap) {
  int clientFd = client.getFd();
  std::string nick = client.getNickname();
  std::ostringstream oss;
  oss << ":" << oldNick << " NICK " + nick + "\r\n";
  std::string message = oss.str();
  channelsMap::const_iterator itEnd = chanMap.end();
  for (channelsMap::const_iterator it = chanMap.begin(); it != itEnd; ++it) {
    if (it->second.isClientInChannel(clientFd)) {
      const clientPMap &clientsInChannel = it->second.getChannelClients();
      clientPMap::const_iterator itClientsInChannelEnd = clientsInChannel.end();
      for (clientPMap::const_iterator itClientsInChannel =
               clientsInChannel.begin();
           itClientsInChannel != itClientsInChannelEnd; ++itClientsInChannel) {
        if (itClientsInChannel->first != clientFd)
          itClientsInChannel->second->receiveMessage(message);
      }
    }
  }
}
