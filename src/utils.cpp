/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/04 21:04:05 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utils.hpp"

#include "../includes/Server.hpp"

void shrink_to_fit(std::vector<struct pollfd> &vec) {
  std::vector<struct pollfd>(vec).swap(vec);
}

bool isNumeric(const std::string& str) {
  for (size_t i = 0; i < str.length(); i++) {
    if (!std::isdigit(str[i])) return false;
  }
  return true;
}

std::string trimWhiteSpaces(const std::string &str) {
  std::string::const_iterator it = str.begin();
  while (it != str.end() && std::isspace(*it))
   ++it;
  std::string trimmed = std::string(it, str.end());
  std::string::reverse_iterator rit = trimmed.rbegin();
  while (rit.base() != trimmed.begin() && std::isspace(*rit))
   ++rit;
  return (std::string(trimmed.begin(), rit.base()));
}
