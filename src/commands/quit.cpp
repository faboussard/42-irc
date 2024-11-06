/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 10:10:53 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/06 11:40:17 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Parser.hpp"

void Server::quit(const std::string &argument, Client *client,
                  clientsMap *cltMap) {
  if (client->isAccepted() == false) {
    clearClient(client->getFd());
    return;
  }
  std::string nick = ":" + client->getNickname() + "!";
  std::string user = client->getUserName();
  std::string host = client->getHostName();
  std::string message = nick + user + "@" + host + " QUIT ";

  if (argument.empty()) {
    message += ": leaving";
  } else {
    if (argument[0] != ':') {
      send461NeedMoreParams(*client, "QUIT");
      return;
    }
    if (argument.substr(0, 2) == "::")
      message += argument.substr(1);
    else
      message += argument;
  }
  message += " ; User " + client->getRealName() + " has quit IRC.";
  message += "\r\n";
  clearClient(client->getFd());
  clientsMap::iterator itEnd = cltMap->end();
  for (clientsMap::iterator it = cltMap->begin(); it != itEnd; ++it) {
    it->second.receiveMessage(message);
  }
}
// format ->
// :<nickname>!<user>@<host> QUIT :<reason>
// :syrk!kalt@millennium.stealth.net QUIT :Gone to have lunch ; User syrk has
// quit IRC to have lunch.
