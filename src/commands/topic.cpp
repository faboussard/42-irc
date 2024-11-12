/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 07:45:39 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/12 17:08:15 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

// Supported command formats:
// - Show current topic of channel #42
//   TOPIC #42
// - Change topic of channel #42 to "new topic"
//   < HexChat > TOPIC #42 new topic (withot :) -- HexChat adds ':'
//   < netcat >  TOPIC #42 :new topic (with :)

bool splitByFirstSpaces(const std::string &arg, stringVector *params,
                        const Client &client) {
  std::string::const_iterator itEnd = arg.end();
  for (std::string::const_iterator it = arg.begin(); it != itEnd; ++it) {
    if (std::isspace(*it)) {
      params->push_back(std::string(arg.begin(), it));
      params->push_back(std::string(it + 1, itEnd));
      if (params->at(1)[0] != ':') {
        send461NeedMoreParams(client, "TOPIC");
        return (false);
      }
      if (params->at(1)[0] == ':' && params->at(1).size() > 1)
        params->at(1) = params->at(1).substr(1);
      else if (params->at(1)[0] == ':' && params->at(1).size() == 1)
        params->at(1) = "";
#ifdef DEBUG
        std::cout << "First: " << params->at(0)
                  << " / Second: " << params->at(1) << std::endl;
#endif
      return (true);
    }
  }
  params->push_back(arg);
#ifdef DEBUG
  std::cout << "First: " << params->at(0) << std::endl;
#endif
  return (true);
}

void Server::topic(int fd, const std::string &arg) {
  Client &client = _clients[fd];
#ifdef DEBUG
  std::cout << "TOPIC command from " << client.getNickname() << std::endl;
#endif
  if (arg.empty()) {
    send461NeedMoreParams(client, "TOPIC");
    return;
  }
  stringVector params;
  if (!splitByFirstSpaces(arg, &params, client)) return;
  const std::string &chanNameWithPrefix = params.at(0);

  if (chanNameWithPrefix[0] != '#') {
    send476BadChanMask(client, chanNameWithPrefix);
  } else if (!findChannel(chanNameWithPrefix)) {
    send403NoSuchChannel(client, chanNameWithPrefix);
  } else {
    const std::string &chanName = chanNameWithPrefix.substr(1);
    Channel &channel = _channels.at(chanName);
    if (!channel.isClientInChannel(fd)) {
      send442NotOnChannel(client, channel);
      return;
    }
    if (params.size() == 1)
      sendTopic(client, channel);
    else
      updateTopic(client, &channel, params.at(1));
  }
}

void Server::sendTopic(const Client &client, const Channel &channel) {
  if (channel.getTopic().topic.empty()) {
    send331Notopic(client, channel);
  } else {
    send332Topic(client, channel);
    send333Topicwhotime(client, channel);
  }
}

void Server::updateTopic(const Client &client, Channel *channel,
                         const std::string &newTopic) {
  if (channel->getMode().topicSettableByOpsOnly &&
      !channel->isOperator(client)) {
    send482ChanOPrivsNeeded(client, *channel);
    return;
  }
  if (newTopic.size() > gConfig->getLimit(TOPICLEN)) {
    send476BadChanMask(client, channel->getNameWithPrefix());
    client.receiveMessage(FROM_SERVER + "NOTICE " +
                          "Topic is too long, limit is " + \
                          gConfig->getParam(TOPICLEN) + " characters.\r\n");
    return;
  }
  channel->setTopic(newTopic, client.getNickname());
  // broadcastTopic(client, *channel);
  broadcastInChannel(client, *channel, "TOPIC", newTopic);
}

// void Server::broadcastTopic(const Client &client, const Channel &channel) {
//   std::string message = ":" + client.getNickname() + " TOPIC " +
//                         channel.getNameWithPrefix() + " :" +
//                         channel.getTopic().topic + "\r\n";
//   const clientPMap &clients = channel.getChannelClients();
//   clientPMap::const_iterator itEnd = clients.end();
//   for (clientPMap::const_iterator it = clients.begin(); it != itEnd; ++it) {
//     it->second->receiveMessage(message);
//   }
// }

// :<client who sent TOPIC> TOPIC <prefix><channel> : <new topic>

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
