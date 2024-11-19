/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 10:10:53 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/19 13:53:14 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

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

