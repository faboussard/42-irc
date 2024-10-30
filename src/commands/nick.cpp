/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/29 14:27:54 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

bool Parser::verifyNick(std::string nick, Client& client, clientsMap cltMap) {
  if (nick.empty()) {
    // sendNumericReply(431, "ERR_NONICKNAMEGIVEN");
    return (false);
  }
  size_t size = nick.size();
  if (size > 9 || std::isdigit(nick[0])) {
    // sendNumericReply(432, "ERR_ERRONEUSNICKNAME");
    return (false);
  }
  for (size_t i = 0; i < size; ++i) {
    if (!std::isalnum(nick[i]) && nick[i] != '[' && nick[i] != ']' &&
        nick[i] != '{' && nick[i] != '}' && nick[i] != '\\' && nick[i] != '|' &&
        nick[i] != '`' && nick[i] != '_' && nick[i] != '^' && nick[i] != '-') {
      // sendNumericReply(432, "ERR_ERRONEUSNICKNAME");
      return (false);
    }
  }
  clientsMap::iterator itEnd = cltMap.end();
  for (clientsMap::iterator it = cltMap.begin(); it != itEnd; ++it) {
    if (it->second.getNickName() == nick) {
      // sendNumericReply(433, "ERR_NICKNAMEINUSE");
      return (false);
    }
  }
  client.setNickname(nick);
  return (true);
}
