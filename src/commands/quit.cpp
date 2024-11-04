/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 10:10:53 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/04 09:30:37 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

void Server::quit(std::string argument, Client &client, clientsMap &cltMap) {
    if (argument.empty()) {
        clearClient(client.getFd());
        return;
    }
    std::string message = argument.substr(argument.find_first_of(":"));
    clearClient(client.getFd());
    // cltMap.erase(client.getFd());
    for (clientsMap::iterator it = cltMap.begin(); it != cltMap.end(); ++it) {
        it->second.receiveMessage("QUIT :" + message + "\r\n");
    }
}