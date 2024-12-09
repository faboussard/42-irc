/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 08:30:30 by mbernard          #+#    #+#             */
/*   Updated: 2024/12/09 10:07:25 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CHANNEL_HPP_
#define INCLUDES_CHANNEL_HPP_

#include <stdint.h>

#include <ctime>
#include <map>
#include <string>
#include <set>

#include "../includes/Config.hpp"

class Client;

typedef std::map<int, Client *> clientPMap;

extern Config *gConfig;

#define REG_CHAN '#'     // regular channel prefix
#define PUBLIC_CHAN '='  // public channel symbol
#define CHAN_OP '@'      // channel operator prefix

typedef struct Topic {
  std::string topic;
  std::string author;
  std::string setTime;
} Topic;

typedef struct Mode {
  bool inviteOnly;
  bool topicSettableByOpsOnly;
  bool keyRequired;
  bool limitSet;
} Mode;

class Channel {
 private:
  std::string _name;
  std::string _type;
  std::string _nameWithPrefix;
  std::string _creationTime;
  Topic _topic;
  Mode _mode;
  std::string _key;
  unsigned int _limit;

  clientPMap _channelClients;
  clientPMap _channelOperators;
  clientPMap _invitedClients;

 public:
  explicit Channel(const std::string &name = "");

  /*  Getters */
  const std::string &getName(void) const;
  const std::string &getType(void) const;
  const std::string getNameWithPrefix(void) const;
  const std::string &getCreationTime(void) const;
  const clientPMap &getChannelClients(void) const;
  const clientPMap &getChannelOperators(void) const;
  const clientPMap &getInvitedClients(void) const;
  const std::set<std::string> &getBannedUsers(void) const;
  const Topic &getTopic(void) const;
  const Mode &getMode(void) const;
  const std::string getChannelModeFlag(void) const;
  const std::string &getKey(void) const;
  unsigned int getLimit(void) const;

  /*  Setters */
  void setTopic(const std::string &topic, const std::string &author);

  /* Clients Management */
  void removeClientFromChannelMap(Client *client);
  void addClientToChannelMap(Client *client);
  void receiveMessageInTheChannel(int fd);
  void checkAndremoveClientFromTheChannel(int fd);
  void addClientToInvitedMap(Client *invited, const std::string &invitingNick);
  void removeClientFromInvitedMap(Client *client);
  bool isClientInChannel(int fd) const;
  bool isClientInvited(int fd) const;

  /* Modes handling */
  // invite-only (i)
  void activateInviteOnlyMode(void);
  void deactivateInviteOnlyMode(void);

  // topic-settable-by-ops-only (t)
  void activateTopicOpsOnlyMode(void);
  void deactivateTopicOpsOnlyMode(void);

  // key-mode (k)
  void updateKey(const std::string &key);
  void activateKeyMode(const std::string &key, const Client &client);
  void deactivateKeyMode(void);

  // operator (o)
  void addOperator(Client *client);
  void removeOperator(Client *client);
  bool isOperator(int fd) const;

  // limit-mode (l)
  void activateLimitMode(unsigned int limit);
  void deactivateLimitMode(void);
};

#endif  // INCLUDES_CHANNEL_HPP_
