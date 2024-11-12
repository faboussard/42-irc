/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/12 12:25:35 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utils.hpp"
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

static std::string trimBeginWithChar(const std::string &str, const char c) {
  std::string::const_iterator it = str.begin();
  std::string::const_iterator itEnd = str.end();

  while (it != itEnd && (*it == c || std::isspace(*it)))
    ++it;

  return (std::string(it, itEnd));
}

std::vector<std::string> split(const std::string& str,
                               const std::string& delim) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delim);
    size_t delimLen = delim.length();
    std::cout << str << std::endl;
    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        token = trimBeginWithChar(token, '\n');
        if (!token.empty()) {
            result.push_back(token);
        }
        start = end + delimLen;
        end = str.find(delim, start);
    }
    std::string token = trimBeginWithChar(str.substr(start), '\n');
    if (!token.empty()) {
        result.push_back(token);
    }
    return (result);
}

commandVectorPairs Parser::parseCommandIntoPairs(const std::string &command) {
  std::vector<std::string> cmds = split(command, "\r\n");
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
    strToUpper(&firstPart);
    pair = std::make_pair(firstPart, secondPart);
    std::cout << CYAN "pair.first : " << pair.first << std::endl;
    std::cout << BLUE "pair.second : " << pair.second << RESET << std::endl;
    result.push_back(pair);
  }
  return (result);
}

