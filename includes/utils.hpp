/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 09:01:51 by mbernard          #+#    #+#             */
/*   Updated: 2024/09/19 09:06:58 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_UTILS_HPP_
#define INCLUDES_UTILS_HPP_

#include <sstream>
#include <string>
#include <vector>

void shrink_to_fit(std::vector<struct pollfd>& vec);

std::string intToString(int number);

#endif  //  INCLUDES_UTILS_HPP_
