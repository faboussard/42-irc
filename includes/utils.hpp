/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 09:01:51 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/31 10:41:16 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_UTILS_HPP_
#define INCLUDES_UTILS_HPP_

#include <sstream>
#include <string>
#include <vector>

void shrink_to_fit(std::vector<struct pollfd> *vec);
void strToUpper(std::string *str);

bool isNumeric(const std::string& str);

template <typename T>
std::string toString(const T& value) {
  std::ostringstream oss;
  oss << value;
  return (oss.str());
}

#endif  //  INCLUDES_UTILS_HPP_
