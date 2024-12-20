/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/12/04 14:13:21 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utils.hpp"

#include <string>
#include <vector>

#include "../includes/Server.hpp"

void shrink_to_fit(std::vector<struct pollfd> *vec) {
  std::vector<struct pollfd>(*vec).swap(*vec);
}

bool isNumeric(const std::string &str) {
  for (size_t i = 0; i < str.length(); i++) {
    if (!std::isdigit(str[i])) return (false);
  }
  return (true);
}

void strToUpper(std::string *str) {
  size_t i = 0;
  size_t len = str->length();
  while (i < len) {
    (*str)[i] = std::toupper((*str)[i]);
    ++i;
  }
}

void capitalize(std::string *str) {
  size_t i = 0;
  size_t len = str->length();
  while (i < len) {
    if (i == 0 || (*str)[i - 1] == ' ') {
      (*str)[i] = std::toupper((*str)[i]);
    } else {
      (*str)[i] = std::tolower((*str)[i]);
    }
    ++i;
  }
}

static std::string trimBeginWithChar(const std::string &str, const char c) {
  std::string::const_iterator it = str.begin();
  std::string::const_iterator itEnd = str.end();

  while (it != itEnd && (*it == c || std::isspace(*it))) ++it;

  return (std::string(it, itEnd));
}

std::vector<std::string> split(const std::string &str,
                               const std::string &delim) {
  std::vector<std::string> result;
  size_t start = 0;
  size_t end = str.find(delim);
  size_t delimLen = delim.length();
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

stringVector basicSplit(const std::string &str) {
  stringVector tokens;
  std::istringstream iss(str);
  std::string token;
  while (iss >> token) {
    tokens.push_back(token);
  }
  return tokens;
}


std::string trimWhiteSpaces(const std::string &input) {
  std::string result = input;
  result.erase(0, result.find_first_not_of(" \t\n\r\f\v"));
  result.erase(result.find_last_not_of(" \t\n\r\f\v") + 1);
  return result;
}

void splitByCommaAndTrim(const std::string &argument, stringVector *args) {
  std::stringstream ss(argument);
  std::string token;
  while (std::getline(ss, token, ',')) {
    args->push_back(trimWhiteSpaces(token));
  }
}

std::string commandToString(Command command) {
  switch (command) {
    case JOIN:
      return ("JOIN");
    case PART:
      return ("PART");
    case KICK:
      return ("KICK");
    case INVITE:
      return ("INVITE");
    case TOPIC:
      return ("TOPIC");
    case MODE:
      return ("MODE");
    case WHO:
      return ("WHO");
    case LIST:
      return ("LIST");
    case NICK:
      return ("NICK");
    case USER:
      return ("USER");
    case PRIVMSG:
      return ("PRIVMSG");
    case QUIT:
      return ("QUIT");
    case PING:
      return ("PING");
    case PASS:
      return ("PASS");
    default:
      return """";
  }
}

std::string commandToString(eBotCommand command) {
  switch (command) {
    case HELLO:
      return ("HELLO");
    case JOKE:
      return ("JOKE");
    case INSULTME:
      return ("INSULTME");
    case ADVICE:
      return ("ADVICE");
    case WEATHER:
      return ("WEATHER");
    default:
      return ("UNKNOWN_BOT_COMMAND");
  }
}
