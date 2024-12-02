/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 09:01:51 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/28 10:09:04 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_UTILS_HPP_
#define INCLUDES_UTILS_HPP_

#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#include "../includes/Server.hpp"
#include "../includes/enums.hpp"

typedef std::vector<std::string> stringVector;
typedef std::pair<std::vector<std::string>, std::vector<std::string> >
    stringVectorPair;

void shrink_to_fit(std::vector<struct pollfd> *vec);
void strToUpper(std::string *str);

bool isNumeric(const std::string &str);
bool isSpecialChar(char c);

std::string trimWhiteSpaces(const std::string &str);

std::vector<std::string> split(const std::string& str,
                               const std::string& delim);

void splitByCommaAndTrim(const std::string &argument, stringVector *args);

stringVector basicSplit(const std::string &str);

std::string commandToString(Command command);
std::string commandToString(eBotCommand command);

template <typename T>
std::string toString(const T &value) {
  std::ostringstream oss;
  oss << value;
  return (oss.str());
}

#endif  //  INCLUDES_UTILS_HPP_
