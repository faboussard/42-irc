/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 08:30:30 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/05 18:23:40 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CHANNEL_HPP_
#define INCLUDES_CHANNEL_HPP_

#include <ctime>
#include <map>
#include <string>

#include "../includes/Client.hpp"
#include "../includes/Config.hpp"

typedef std::map<int, Client*> clientPMap;

extern Config *gConfig;

#define REG_CHAN "#"     // regular channel prefix
#define PUBLIC_CHAN "="  // public channel symbol
#define CHAN_OP "@"      // channel operator prefix

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
  std::string key;
  int limit;
} Mode;

class Channel {
 private:
  std::string _name;
  std::string _type;
  std::string _nameWithPrefix;
  std::string _creationTime;
  Topic       _topic;
  Mode        _mode;

std::map<int, Client*> _clientsInChannel;
 std::map<int, Client*> _channelOperators;

 public:
  explicit Channel(const std::string &name = "");
  bool isSecret; // sera remplacé dans la strcuture de Yuko. 

  /* Getters */

  const std::string &getName() const;
  const std::string getNameWithPrefix() const;
  const std::string &getCreationTime() const;
  const  std::map<int, Client*> &getClientsInChannel() const;
  const  std::map<int, Client*> &getChannelOperators() const;
  const Topic &getTopic(void) const;
  const Mode &getMode(void) const;
  const std::string getChannelModeFlag(void) const;
  const std::string &getKey(void) const;
  int getLimit(void) const;

  /* Setters */

  void setTopic(const std::string &topic, const std::string &author);
  void setInviteOnlyMode(void);
  void setTopicSettableByOpsOnlyMode(void);
  // void setLimit(int limit, const Client &cli);

  /* Member Functions */

  void removeClientFromTheChannel(int fd);
  void addClientToChannelMap(Client *client); 
  void receiveMessageInTheChannel(int fd);

  void addOperator(Client *client);

  void updateKey(const std::string &key);
  void activateKeyMode(const std::string &key, const Client &client);
  void deactivateKeyMode(void);
  void activateLimitMode(int limit, const Client &client);
  void deactivateLimitMode(void);
};

#endif  // INCLUDES_CHANNEL_HPP_
