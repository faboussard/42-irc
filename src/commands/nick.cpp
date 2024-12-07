/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/12/06 09:34:01 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>

#include "../../includes/Log.hpp"
#include "../../includes/Parser.hpp"
#include "../../includes/colors.hpp"

static bool nickAlreadyUsed(clientsMap::iterator it, const std::string &nick);
static void broadcastNickChange(const Client &client,
                                const std::string &oldNick,
                                const channelsMap &chanMap,
                                const clientsMap &clientsMap);
static void fetchClientChannels(int clientFd, const channelsMap &chanMap,
                                std::vector<const Channel *> *channels);

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
  if (alreadyRegisterd) broadcastNickChange(*client, oldNick, chanMap, *cltMap);
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

void broadcastNickChange(const Client &client, const std::string &oldNick,
                         const channelsMap &chanMap,
                         const clientsMap &clientsMap) {
  std::ostringstream oss;
  oss << ":" << oldNick << " NICK " + client.getNickname() + "\r\n";
  std::string message = oss.str();
  std::vector<const Channel *> channels;
  fetchClientChannels(client.getFd(), chanMap, &channels);

  // Check through all clients
  clientsMap::const_iterator itEnd = clientsMap.end();
  for (clientsMap::const_iterator it = clientsMap.begin(); it != itEnd; ++it) {
    if (it->second.getFd() == client.getFd()) continue;
    // Check if they are in the same channel as the client
    std::vector<const Channel *>::const_iterator itEndChannels = channels.end();
    for (std::vector<const Channel *>::const_iterator itChannels =
             channels.begin();
         itChannels != itEndChannels; ++itChannels) {
      if ((*itChannels)->isClientInChannel(it->second.getFd())) {
        it->second.receiveMessage(message);
        break;
      }
    }
  }
  client.receiveMessage(message);
}

void fetchClientChannels(int clientFd, const channelsMap &chanMap,
                         std::vector<const Channel *> *channels) {
  channelsMap::const_iterator itEnd = chanMap.end();
  for (channelsMap::const_iterator it = chanMap.begin(); it != itEnd; ++it) {
    if (it->second.isClientInChannel(clientFd)) {
      channels->push_back(&it->second);
    }
  }
#ifdef DEBUG
  std::ostringstream oss;
  oss << "Client's channels: ";
  for (size_t i = 0; i < channels->size(); ++i) {
    oss << channels->at(i)->getName() << " ";
  }
  Log::printLog(DEBUG_LOG, CLIENT, oss.str());
#endif
}
