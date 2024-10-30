/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/30 10:47:43 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

bool Parser::verifyPassword(std::string arg, std::string psd, Client& client) {
  if (client.isPasswordGiven()) {
    send462AlreadyRegistered(client.getFd(), client.getNickName());
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
  return (true);
}
