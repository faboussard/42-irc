/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 11:04:35 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/07 13:49:42 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

// WHO
// WHO <#channel>
// WHO <#channel> <o>

// After JOIN #chanName --> Hexchat sends MODE #chanName & WHO #chanName

void Server::who(const Client &client, const std::string &arg) {
  if (arg.empty()) {
    // List of all clients in all channels
  }
}
