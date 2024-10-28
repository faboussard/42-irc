/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 08:30:30 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/28 15:33:43 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CHANNEL_HPP_
#define INCLUDES_CHANNEL_HPP_

#include <ctime>
#include <map>
#include <string>

#include "../includes/Client.hpp"

typedef std::map<int, Client> clientsMap;

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
  Topic _topic;
  Mode _mode;

  clientsMap _clientsInChannel;
  clientsMap _channelOperators;

 public:
  explicit Channel(const std::string &name = "");

  /* Getters */

  const std::string &getName() const;
  const std::string getNameWithPrefix() const;
  const clientsMap &getClientsInChannel() const;
  const clientsMap &getChannelOperators() const;
  const Topic &getTopic(void) const;
  const Mode &getMode(void) const;
  const std::string &getKey(void) const;
  int getLimit(void) const;

  /* Setters */

  void setTopic(const std::string &topic, const std::string &author);
  void setInviteOnlyMode(void);
  void setTopicSettableByOpsOnlyMode(void);
  // void setKey(const std::string &key, const Client &cli);
  // void setLimit(int limit, const Client &cli);

  /* Member Functions */

  void removeClientFromTheChannel(int fd);
  void acceptClientInTheChannel(const Client &client);  // Utilisation du type défini
  void receiveMessageInTheChannel(int fd);

  void activateKeyMode(const std::string &key, const Client &cli);
  void deactivateKeyMode(void);
  void activateLimitMode(int limit, const Client &cli);
  void deactivateLimitMode(void);
};

#endif  // INCLUDES_CHANNEL_HPP_
