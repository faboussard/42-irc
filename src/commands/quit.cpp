/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 10:10:53 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/05 13:40:37 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Parser.hpp"

void Server::quit(const std::string &argument, Client &client, clientsMap &cltMap) {
    if (client.isAccepted() == false) {
        clearClient(client.getFd());
        return;
    }
    std::string nick = ":" + client.getNickname() + " ";
    std::string message = "QUIT ";
// :syrk!kalt@millennium.stealth.net QUIT :Gone to have lunch ; User syrk has quit IRC to have lunch.


    if (argument.empty())
      message += ": leaving";
    else
      message += argument.substr(argument.find_first_of(":"));
    // std::string message = argument.substr(argument.find_first_of(":"));
    message += " ; User " + client.getRealName() + " has quit IRC";
    message += "\r\n";
    clearClient(client.getFd());
    // cltMap.erase(client.getFd());
    for (clientsMap::iterator it = cltMap.begin(); it != cltMap.end(); ++it) {
        it->second.receiveMessage(nick + message);
    }
}
