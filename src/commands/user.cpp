/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/31 08:55:10 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

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
  std::string lastToken = str.substr(str.find_first_of(":"));
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

static bool realnameIsInvalid(const std::string realname) {
  size_t size = realname.size();

  for (size_t i = 0; i < size; ++i) {
    if (std::iscntrl(realname.at(i))) {
      return (true);
    }
  }
  return (false);
}

bool Parser::verifyUser(std::string user, Client &client, clientsMap cltMap) {
  if (client.isUsernameSet()) {
    send462AlreadyRegistered(client.getFd(), client.getNickName());
    return (false);
  }
  std::vector<std::string> fields = fillUserVectorString(user);
  if (user.empty() || fields.size() != 4) {
    send461NeedMoreParams(client.getFd(), client.getNickName(), "USER");
    return (false);
  }
  if (usernameIsInvalid(fields[0]) || realnameIsInvalid(fields[3])) {
    send432ErroneusNickname(client.getFd(), client.getNickName());
    return (false);
  }
  clientsMap::iterator itEnd = cltMap.end();
  for (clientsMap::iterator it = cltMap.begin(); it != itEnd; ++it) {
    if (it->second.getUserName() == user) {
      send462AlreadyRegistered(client.getFd(), client.getNickName());
      return (false);
    }
  }
  client.setUserName(user);
  client.setRealName(fields[3]);
  return (true);
}
