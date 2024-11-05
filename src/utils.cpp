/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/31 10:41:02 by yusengok         ###   ########.fr       */
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
