/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/08 15:26:07 by faboussa         ###   ########.fr       */
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

std::string trimWhiteSpaces(const std::string &input) {
    std::string result = input;
    result.erase(0, result.find_first_not_of(" \t\n\r\f\v"));  // Retirer les espaces au début
    result.erase(result.find_last_not_of(" \t\n\r\f\v") + 1);  // Retirer les espaces à la fin
    return result;
}

// std::string trimWhiteSpaces(const std::string &str) {
//   std::string::const_iterator itBegin = str.begin();
//   std::string::const_iterator itEnd = str.end();
//   std::string::const_iterator it = itBegin;

//   while (it != itEnd && std::isspace(*it)) ++it;
//   std::string::const_reverse_iterator rit = str.rbegin();
//   while (rit.base() != itBegin && std::isspace(*rit)) ++rit;
//   return (std::string(it, rit.base()));
// }

void splitByCommaAndTrim(const std::string &argument, stringVector *args) {
  std::stringstream ss(argument);
  std::string token;
  while (std::getline(ss, token, ',')) {
    args->push_back(trimWhiteSpaces(token));
  }
}
