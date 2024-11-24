/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/24 13:54:51 by fanny            ###   ########.fr       */
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

KeyValuePairList parseCommandIntoKeyValuePairList(const std::string &key,
                                                  const std::string &value) {
  stringVector keyVector;
  stringVector valueVector;
  KeyValuePairList list;
  splitByCommaAndTrim(key, &keyVector);

#ifdef DEBUG
  {
    std::ostringstream before, after;
    before << "key: Before split and trim key: " << key;
    after << "keyVector: After split and trim keyVector: ";
    for (size_t i = 0; i < keyVector.size(); ++i) after << keyVector[i] << "|";
    Server::printLog(DEBUG_LOG, COMMAND, before.str());
    Server::printLog(DEBUG_LOG, COMMAND, after.str());
  }
#endif
  std::cout << "keyVector.size(): " << keyVector.size() << "and stdcount"
            << std::count(key.begin(), key.end(), ',') + 1 << std::endl;
  if (keyVector.size() != static_cast<std::vector<std::string>::size_type>(
                              std::count(key.begin(), key.end(), ',') + 1)) {
    return KeyValuePairList();
  }
  splitByCommaAndTrim(value, &valueVector);
#ifdef DEBUG
  {
    std::ostringstream before, after;
    before << "value: Before split and trim value: " << value;
    after << "valueVector: After split and trim valueVector: ";
    for (size_t i = 0; i < valueVector.size(); ++i)
      after << valueVector[i] << "|";
    Server::printLog(DEBUG_LOG, COMMAND, before.str());
    Server::printLog(DEBUG_LOG, COMMAND, after.str());
  }
#endif
  list.first = keyVector;
  list.second = valueVector;
  return (list);
}
