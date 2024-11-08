/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 07:45:39 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/08 09:51:27 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

void Server::topic(int fd, const std::string &arg) {
/*--- To remove ---*/
  (void)fd;
  (void)arg;
/*-----------------*/

//   const Client &client = _clients.at(fd);
//   stringVector params;
}

// ======= TOPIC from a client who is not on the channel =======================
// Reply:
// -- 442 ERR_NOTONCHANNEL

// ======= TOPIC with 't' mode from a client on the channel but not ops ========
// Reply:
//  // -- 482 ERR_CHANOPRIVSNEEDED:

// ======= TOPIC <channel> =====================================================
// Replies:
// -- 332 RPL_TOPIC & RPL_TOPICWHOTIME:
// If the client is on the channel && the topic is set

// -- 331 RPL_NOTOPIC
// If the topic is not set

// -- 403 ERR_NOSUCHCHANNEL
// If the channel does not exist

// ======= TOPIC <channel> :<empty topic> ======================================
// Replies:
// -- 482 ERR_CHANOPRIVSNEEDED:
// If the channel is on 't' mode && TOPIC was sent
// from a client on the channel but not ops

// -- :<client who sent TOPIC> TOPIC <prefix><channel> :
// Topic for the channel will be cleared.
// Replies (not numeric) to every client in that channel
// (including the author of the topic change)

// ======= TOPIC <channel> :<topic> ============================================
// Replies:
// -- 482 ERR_CHANOPRIVSNEEDED:
// If the channel is on 't' mode && TOPIC was sent
// from a client on the channel but not ops

// -- :<client who sent TOPIC> TOPIC <prefix><channel> : <new topic>
// Topic of a channel will be changed.
// Replies (not numeric) to every client in that channel
// (including the author of the topic change)
// e.g. [:Alice TOPIC #test :New topic]

// If the <topic> param is provided but the same as the previous topic
// (ie. it is unchanged),
// servers MAY notify the author and/or other users anyway.
