/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/12 11:54:00 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Parser.hpp"
#include "../../includes/colors.hpp"

static std::vector<std::string> fillUserVectorString(const std::string &str) {
  if (str.empty()) return (std::vector<std::string>());
  std::vector<std::string> result;
  std::istringstream iss(str);
  std::string token;
  int8_t counter = 0;

  while (iss >> token && counter < 4) {
    result.push_back(token);
    ++counter;
  }
  if (counter < 4 || result.at(3)[0] != ':') {
    result.clear();
    return (result);
  }
  std::string lastToken = str.substr(str.find_first_of(":") + 1);
  if (lastToken.empty() || lastToken.size() > 100)
    result.clear();
  else
    result.at(3) = lastToken;
  return (result);
}

static bool usernameIsInvalid(const std::string &username) {
  const size_t size = username.size();

  if (size > 10) {
    return (true);
  }
  for (size_t i = 0; i < size; ++i) {
    if (std::isspace(username.at(i)) || std::iscntrl(username.at(i))) {
      return (true);
    }
  }
  return (false);
}

static bool realnameIsInvalid(const std::string &realname) {
  size_t size = realname.size();

  for (size_t i = 0; i < size; ++i) {
    if (std::iscntrl(realname.at(i))) {
      return (true);
    }
  }
  return (false);
}

bool Parser::verifyUser(const std::string &user, Client *client,
                        clientsMap *cltMap) {
  if (client->isUsernameSet()) {
    send462AlreadyRegistered(*client);
    return (false);
  }
  std::vector<std::string> fields = fillUserVectorString(user);
  if (user.empty() || fields.size() != 4) {
    send461NeedMoreParams(*client, "USER");
    return (false);
  }
  if (usernameIsInvalid(fields[0]) || realnameIsInvalid(fields[3])) {
    send432ErroneusNickname(*client, client->getNickname());
    return (false);
  }
  clientsMap::iterator itEnd = cltMap->end();
  for (clientsMap::iterator it = cltMap->begin(); it != itEnd; ++it) {
    if (it->second.getUserName() == user) {
      send462AlreadyRegistered(*client);
      return (false);
    }
  }
  client->setUserName(fields[0]);
  client->setRealName(fields[3]);
  std::cout << BRIGHT_YELLOW "UserName IS ACCEPTED !!!!! : "
            << client->getUserName() << RESET << std::endl;
  return (true);
}
