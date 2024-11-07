/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/07 12:37:49 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utils.hpp"
#include "../includes/Server.hpp"

void shrink_to_fit(std::vector<struct pollfd> *vec) {
  std::vector<struct pollfd>(*vec).swap(*vec);
}

bool isNumeric(const std::string& str) {
  for (size_t i = 0; i < str.length(); i++) {
    if (!std::isdigit(str[i])) return false;
  }
  return true;
}

void strToUpper(std::string *str) {
  size_t i = 0;
  size_t len = str->length();
  while (i < len) {
    (*str)[i] = std::toupper((*str)[i]);
    ++i;
  }
}