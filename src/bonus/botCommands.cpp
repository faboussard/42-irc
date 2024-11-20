/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 07:52:23 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/20 09:44:12 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"
#include "../../includes/Server.hpp"

void Server::botCommands(Client *client, Command command,
                         const std::string &arg) {
  (void)arg;
  if (command == BOT) {
    // sendBotCommandsList(client);
       client->setBotLaunched(true);
  } else {
#ifdef DEBUG
    std::ostringstream oss;
    oss << "Bot command received from " << client->getNickname();
    printLog(DEBUG_LOG, BOT_L, oss.str());
#endif
    if (client->botLaunched()) {
      // sendToBot(client, command, arg);
    } else {
      // sendBotNotLaunched(client);
    }
  }
}
