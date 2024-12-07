/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/12/05 21:51:00 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

#include "../../includes/Parser.hpp"

#include "../../includes/Client.hpp"
#include "../../includes/Log.hpp"
#include "../../includes/colors.hpp"

static void logPassAuthFailed(const Client &client);
static void logPassAuthSuccess(const Client &client);

bool Parser::verifyPassword(std::string arg, std::string psd, Client *client) {
  if (client->isPasswordGiven()) {
    send462AlreadyRegistered(*client);
    return (false);
  }
  client->incrementNbPassAttempts();
  if (arg.empty()) {
    logPassAuthFailed(*client);
    send461NeedMoreParams(*client, "PASS");
    return (false);
  }
  if (arg != psd) {
    logPassAuthFailed(*client);
    send464PasswdMismatch(*client);
    return (false);
  }
  client->declarePasswordGiven();
  logPassAuthSuccess(*client);
  return (true);
}

void logPassAuthFailed(const Client &client) {
  int remainingAttempts = 3 - client.getNbPassAttempts();
  std::ostringstream oss;
  oss << client.getNickname() << " (fd" << client.getFd()
      << "): Password authentication failed. " << remainingAttempts
      << " attempt(s) remaining.";
  if (remainingAttempts == 0)
    Log::printLog(WARNING_LOG, AUTH, oss.str());
  else
    Log::printLog(NOTIFY_LOG, AUTH, oss.str());
}

void logPassAuthSuccess(const Client &client) {
  std::ostringstream oss;
  oss << client.getNickname() << " (fd" << client.getFd()
      << "): Password authentication successful";
  Log::printLog(INFO_LOG, AUTH, oss.str());
}
