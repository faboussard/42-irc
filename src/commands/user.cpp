/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/30 10:48:28 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

static std::vector<std::string> fillUserVectorString(const std::string str) {
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

static bool usernameContainsForbiddenCaracters(const std::string username) {
  size_t size = username.size();

  for (size_t i = 0; i < size; ++i) {
    if (std::isspace(username.at(i)) || std::iscntrl(username.at(i))) {
      std::cout << "space or control char" << std::endl;
    // ERR_ERRONEUSNICKNAME (432)
      return (true);
    }
  }
  return (false);
}

static bool realnameContainsForbiddenCaracters(const std::string realname) {
  size_t size = realname.size();

  for (size_t i = 0; i < size; ++i) {
    if (std::iscntrl(realname.at(i))) {
      std::cout << "control char" << std::endl;
    // ERR_ERRONEUSNICKNAME (432)
      return (true);
    }
  }
  return (false);
}

bool Parser::verifyUser(std::string user, Client& client, clientsMap cltMap) {
  if (client.isUsernameSet()) {
    // ERR_ALREADYREGISTERED (462)
    return (false);
  }
  if (user.empty()) {
    // ERR_NEEDMOREPARAMS (461)
    return (false);
  }
  std::vector<std::string> fields = fillUserVectorString(user);
  if (fields.size() != 4) {
    std::cout << "fields.size() != 4" << std::endl;
    // ERR_NEEDMOREPARAMS (461)
    return (false);
  }
  if (usernameContainsForbiddenCaracters(fields[0])
      || realnameContainsForbiddenCaracters(fields[3]))
    return (false);
  clientsMap::iterator itEnd = cltMap.end();
  for (clientsMap::iterator it = cltMap.begin(); it != itEnd; ++it) {
    if (it->second.getUserName() == user) {
      // sendNumericReply(462, "ERR_ALREADYREGISTERED");
      std::cout << "username already registered" << std::endl;
      return (false);
    }
  }
  client.setUserName(user);
  client.setRealName(fields[3]);
  return (true);
}
