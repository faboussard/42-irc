/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/12/05 20:57:32 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "../../includes/Parser.hpp"

#include "../../includes/Log.hpp"
#include "../../includes/colors.hpp"

bool Parser::verifyNick(const std::string &nick, Client *client,
                        clientsMap *cltMap) {
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
    std::string nickToCompare = it->second.getNickname();
    std::string requestedNick = nick;
    strToUpper(&nickToCompare);
    strToUpper(&requestedNick);
#ifdef DEBUG
    std::ostringstream oss;
    oss << "Comparing " << nickToCompare << " with " << requestedNick;
    Log::printLog(DEBUG_LOG, CLIENT, oss.str());
#endif
    // if (it->second.getNickname() == nick) {
    if (nickToCompare == requestedNick) {
      if (it->second.getFd() != client->getFd())
        send433NickAlreadyInUse(*client, nick);
      return (false);
    }
  }
  client->setNickname(nick);
  std::ostringstream oss;
  oss << client->getNickname() << " (fd" << client->getFd()
      << "): Nickname validated";
  Log::printLog(INFO_LOG, CLIENT, oss.str());
  return (true);
}
