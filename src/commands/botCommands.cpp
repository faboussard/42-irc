/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 07:52:23 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/26 16:23:12 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <poll.h>

#include <algorithm>
#include <string>
#include <vector>

#include "../../includes/Bot.hpp"
#include "../../includes/Server.hpp"

void logRequestSentToBot(const std::string &nick, Command command,
                         const std::string &arg);
#ifdef DEBUG
static void logBotCommand(const std::string &nick, Command command,
                          const std::string &arg);
#endif

/*============================================================================*/
/*       Bot command handling                                                 */
/*============================================================================*/

void Server::botCommands(Client *client, Command command,
                         const std::string &arg) {
  if (_bot == NULL) {
    sendNotice(*client, "Bot not available. Please try again later.");
    return;
  }
#ifdef DEBUG
  logBotCommand(client->getNickname(), command, arg);
#endif
  if (client->getNickname() == BOT_NAME)
    return;
  if (command == BOT) {
    // sendBotInstruction(*client);
    if (!client->botLaunched()) {
      client->setBotLaunched(true);
      // send command BOT to ircbot (and bot reply with welcome & instructions)
      sendRequestToBot(*client, command, "welcome");
    } else {
      // send command BOT to ircbot (and bot reply with instructions)
      sendRequestToBot(*client, command, "menu");
    } 
  } else {
    if (client->botLaunched()) {
      sendRequestToBot(*client, command, arg);
    } else {
      sendNotice(*client, "Bot not launched yet. Please type /BOT to launch.");
    }
  }
}

// (NUMBERS nick arg)

// void Server::sendBotInstruction(const Client &client) {
//   std::string clientNickname = client.getNickname();
//   const stringVector &instructions = _bot->getInstructions();
//   stringVector::const_iterator itEnd = instructions.end();
//   for (std::vector<std::string>::const_iterator it = instructions.begin();
//        it != itEnd; ++it) {
//     std::ostringstream oss;
//     oss << BOT_RESPONSE_HEADER << clientNickname << " :" << *it << "\r\n";
//     client.receiveMessage(oss.str());
//   }
// }

void Server::sendRequestToBot(const Client &client, Command command,
                              const std::string &arg) {
  const std::string &nick = client.getNickname();
  _bot->receiveMessage(commandToString(command) + " " + nick + " " + arg);
  logRequestSentToBot(nick, command, arg);
}

/*============================================================================*/
/*       Logs                                                                 */
/*============================================================================*/

void logRequestSentToBot(const std::string &nick, Command command,
                         const std::string &arg) {
  std::ostringstream oss;
  oss << "IRC Server has sent a request from " << nick
      << commandToString(command) << " " << arg;
  Server::printLog(INFO_LOG, BOT_L, oss.str());
}

#ifdef DEBUG
void logBotCommand(const std::string &nick, Command command,
                   const std::string &arg) {
  std::ostringstream oss;
  oss << "Server has received from " << nick << ": Command " << CYAN
      << Bot::commandToString(command) << RESET " | " << "arg " << CYAN << arg
      << RESET;
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif
