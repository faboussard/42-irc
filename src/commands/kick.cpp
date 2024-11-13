/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:20:03 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/13 11:04:59 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"
  /*
  commentaire pour PR: a enlever apres*/

// articulation de la fonction kick: 
// kick #channelName #targetNick #reason (reason is optional)

void Server::parseKickParams(const std::string &param, Client &client,
                             std::string &channelName, std::string &targetNick,
                             std::string &reason)
{
    if (param.empty()) {
    send461NeedMoreParams(client, "KICK");
    return;
  }

  /*
  Si aucun espace n'est trouvé dans la chaîne param, cette ligne envoie un
  message d'erreur au client, indiquant qu'il manque des paramètres pour la
  commande KICK.*/
  std::string::size_type spacePos = param.find(" ");
  if (spacePos == std::string::npos) {
    send461NeedMoreParams(client, "KICK");
    return;
  }
  
    if (spacePos != std::string::npos) {
    channelName = param.substr(0, spacePos);
    std::string::size_type spacePos2 = param.find(" ", spacePos + 1);
    if (spacePos2 != std::string::npos) {
      targetNick = param.substr(spacePos + 1, spacePos2 - spacePos - 1);
      reason = param.substr(spacePos2 + 1);
    } else {
      targetNick = param.substr(spacePos + 1);
    }
  }
}

void Server::kick(int fd, const std::string &param) {
  Client &client = _clients.at(fd);

  std::string channelName;
  std::string targetNick;
  std::string reason;

  parseKickParams(param, client, channelName, targetNick, reason);
  

}
