/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/01 23:30:07 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Parser.hpp"
#include "../../includes/colors.hpp"

bool Parser::verifyPassword(std::string arg, std::string psd, Client& client) {
  if (client.isPasswordGiven()) {
    send462AlreadyRegistered(client.getFd(), client.getNickname());
    return (false);
  }
  if (arg.empty()) {
    send461NeedMoreParams(client.getFd(), "", "PASS");
    return (false);
  }
  if (arg != psd) {
    send464PasswdMismatch(client.getFd(), "");
    return (false);
  }
  client.declarePasswordGiven();
  std::cout << BRIGHT_YELLOW "Password IS ACCEPTED !!!!! : " << arg << RESET << std::endl;
  return (true);
}
