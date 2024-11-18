/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/18 13:19:23 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "../../includes/Parser.hpp"
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
    if (it->second.getNickname() == nick) {
      if (it->second.getFd() != client->getFd())
        send433NickAlreadyInUse(*client, nick);
      return (false);
    }
  }
  client->setNickname(nick);
  // std::cout << BBRIGHT_YELLOW "NickName IS ACCEPTED !!!!! : "
  //           << client->getNickname() << RESET << std::endl;
  std::ostringstream oss;
  oss << client->getNickname() << " (fd" << client->getFd()
      << "): Nickname validated";
  Server::printLog(INFO_LOG, CLIENT, oss.str());
  return (true);
}
