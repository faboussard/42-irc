/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:17:50 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/04 13:55:41 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

void Server::list(Client *client, const std::string argument) {
  int fd = client->getFd();
  std::string nick = client->getNickname();
  send321Liststart(fd, nick);
  // If no argument is given, list all channels

  // If ageument begins with a '#', list the channels with the given names

  //  

  send323Listend(fd, nick);
}