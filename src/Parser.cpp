/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/05 10:58:53 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

#include "../includes/colors.hpp"

Command Parser::choseCommand(const std::string& command) {
  if (command == "JOIN") {
    return (JOIN);
  } else if (command == "KICK") {
    return (KICK);
  } else if (command == "INVITE") {
    return (INVITE);
  } else if (command == "TOPIC") {
    return (TOPIC);
  } else if (command == "MODE") {
    return (MODE);
  } else if (command == "LIST") {
    return (LIST);
  } else if (command == "NOTICE") {
    return (NOTICE);
  } else if (command == "NICK") {
    return (NICK);
  } else if (command == "PRIVMSG") {
    return (PRIVMSG);
  } else if (command == "QUIT") {
    return (QUIT);
  } else if (command == "PING") {
    return (PING);
  } else if (command == "CAP") {
    return (CAP);
  } else if (command == "USER") {
    return (USER);
  } else if (command == "PASS") {
    return (PASS);
  }
  return (UNKNOWN);
}

std::vector<std::string> Parser::splitCommand(const std::string& command) {
  std::istringstream iss(command);
  std::string token;
  std::vector<std::string> message;

  while (iss >> token) {
    message.push_back(token);
    token.clear();
  }
  return (message);
}

std::vector<std::string> split(const std::string& str, char delim1,
                               char delim2) {
  std::vector<std::string> result;
  std::string strWithoutDelim2 = str;
  size_t size = str.size();

  for (size_t i = 0; i < size; ++i) {
    if (strWithoutDelim2[i] == delim2) {
      strWithoutDelim2[i] = delim1;
    }
  }
  std::stringstream ss(strWithoutDelim2);
  std::string item;

  while (getline(ss, item, delim1)) {
    if (item.empty()) continue;
    while (std::isspace(item[0])) {
      item.erase(0, 1);
    }
    if (item.empty()) continue;
    result.push_back(item);
  }
  return (result);
}

commandVectorPairs Parser::parseCommandIntoPairs(std::string command) {
  std::vector<std::string> cmds = split(command, '\n', '\r');
  commandVectorPairs result;
  std::string token;
  std::pair<std::string, std::string> pair;
  size_t size = cmds.size();

  for (size_t i = 0; i < size; ++i) {
    cmds[i].erase(cmds[i].find_last_not_of(" \n\r\t") + 1);
    std::string firstPart = cmds[i].substr(0, cmds[i].find_first_of(" "));
    std::string secondPart;
    if (firstPart.size() != cmds[i].size())
      secondPart = cmds[i].substr(cmds[i].find_first_of(" ") + 1);
    else
      secondPart = "";
    pair = std::make_pair(firstPart, secondPart);
    std::cout << CYAN "pair.first : " << pair.first << std::endl;
    std::cout << BLUE "pair.second : " << pair.second << RESET << std::endl;
    result.push_back(pair);
  }
  return (result);
}

