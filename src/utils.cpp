/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/17 15:12:35 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utils.hpp"

#include "../includes/Server.hpp"

void shrink_to_fit(std::vector<struct pollfd> &vec) {
  std::vector<struct pollfd>(vec).swap(vec);
}

std::string concatenate(const std::string& first) {
    return (first);
}

std::string intToString(int number) {
  std::ostringstream oss;
  oss << number;
  return (oss.str());
}
