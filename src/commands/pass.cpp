/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/30 09:00:03 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

bool Parser::verifyPassword(std::string arg, std::string psd, Client& client) {
  if (client.isPasswordGiven()) {
    // sendNumericReply(462, "ERR_ALREADYREGISTERED");
    return (false);
  }
  if (arg.empty()) {
    // sendNumericReply(461, "ERR_NEEDMOREPARAMS");
    return (false);
  }
  if (arg != psd) {
    // sendNumericReply(464, "ERR_PASSWDMISMATCH");
    return (false);
  }
  client.declarePasswordGiven();
  return (true);
}

