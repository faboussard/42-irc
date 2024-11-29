/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 07:45:39 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/28 18:33:45 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>

#include "../../includes/Server.hpp"

void Server::topic(int fd, const std::string &arg) {
  Client &client = _clients.at(fd);
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "TOPIC: Command received from " << client.getNickname();
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  if (arg.empty()) {
    send461NeedMoreParams(client, "TOPIC");
    return;
  }
  stringVector params;
  if (!parseTopicParams(arg, &params, client)) return;
  const std::string &chanNameWithPrefix = params.at(0);
  if (!channelExists(chanNameWithPrefix)) {
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

bool Server::parseTopicParams(const std::string &arg, stringVector *params,
                              const Client &client) {
  std::istringstream iss(arg);
  std::string channel, topic;
  iss >> channel;
  if (channel.empty()) {
    send461NeedMoreParams(client, "TOPIC");
    return (false);
  }
  if (channel[0] != REG_CHAN) {
    send476BadChanMask(client, channel);
    return (false);
  }
  params->push_back(channel);
  std::getline(iss >> std::ws, topic);
  if (topic.empty()) return (true);
  if (topic[0] != ':') {
    send461NeedMoreParams(client, "TOPIC");
    return (false);
  }
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "TOPIC: Channel: " << channel << " / Topic before trim: " << topic;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  topic.erase(0, topic.find_first_not_of(":"));
#ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "TOPIC: Topic after trim: " << topic;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  if (topic.empty()) topic = "";
  params->push_back(topic);
  return (true);
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
      !channel->isOperator(client.getFd())) {
    send482ChanOPrivsNeeded(client, *channel);
    return;
  }
  if (newTopic.size() > gConfig->getLimit(TOPICLEN)) {
    send400UnknownError(client, channel->getNameWithPrefix(), "Topic too long");
    return;
  }
  std::string oldTopic = channel->getTopic().topic;
  channel->setTopic(newTopic, client.getNickname());
  if (oldTopic.empty() && newTopic.empty())
    send331Notopic(client, *channel);
  else
    broadcastInChannelExceptToSender(client, *channel, "TOPIC", newTopic);
}
