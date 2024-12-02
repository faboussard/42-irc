/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/12/02 21:48:16 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "../includes/Log.hpp"
#include "../includes/colors.hpp"
#include "../includes/enums.hpp"
#include "../includes/utils.hpp"

Command Parser::choseCommand(const std::string& command) {
  if (command == "JOIN") {
    return (JOIN);
  } else if (command == "PART") {
    return (PART);
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
  } else if (command == "NICK") {
    return (NICK);
  } else if (command == "PRIVMSG") {
    return (PRIVMSG);
  } else if (command == "QUIT") {
    return (QUIT);
  } else if (command == "PING") {
    return (PING);
  } else if (command == "PONG") {
    return (PONG);
  } else if (command == "CAP") {
    return (CAP);
  } else if (command == "USER") {
    return (USER);
  } else if (command == "PASS") {
    return (PASS);
  } else if (command == "WHO") {
    return (WHO);
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

stringPairsVector Parser::parseCommandIntoPairs(const std::string& command) {
  std::vector<std::string> cmds = split(command, "\r\n");
  stringPairsVector result;
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
    strToUpper(&firstPart);
    pair = std::make_pair(firstPart, secondPart);
    std::ostringstream oss;
    oss << "pair.first: " CYAN << pair.first << RESET " | pair.second: " CYAN
        << pair.second << RESET;
    Log::printLog(DEBUG_LOG, PARSER, oss.str());
    result.push_back(pair);
  }
  return (result);
}
