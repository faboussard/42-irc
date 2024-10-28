/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 08:30:30 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/28 14:43:01 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CHANNEL_HPP_
#define INCLUDES_CHANNEL_HPP_

#include <ctime>
#include <map>
#include <string>

#include "../includes/Client.hpp"

#define CHANLIMIT 3

typedef std::map<int, Client> clientsMap;

typedef struct Topic {
  std::string topic;
  std::string author;
  std::string setTime;
} Topic;

class Channel {
 private:
  std::string _name;
  // std::string _topic;
  Topic _topic;
  bool _inviteOnly;
  bool _topicSettableByOpsOnly;
  std::string _key;
  int _limit;

  clientsMap _clientsInChannel;

 public:
  explicit Channel(const std::string &name = "");
  bool isSecret; // sera remplacé dans la strcuture de Yuko. 


  /* Getters */

  const std::string &getName() const;
  const clientsMap &getClientsInChannel() const;
  // const std::string &getTopic() const;
  const Topic &getTopic(void) const;
  bool getInviteOnlyModeStatus(void) const;
  bool getTopicSettableByOpsOnlyModeStatus(void) const;
  const std::string &getKey(void) const;
  int getLimit(void) const;

  /* Setters */

  // void setTopic(const std::string &topic);
  void setTopic(const std::string &topic, const std::string &author);
  void setInviteOnlyMode(bool inviteOnly);
  void setTopicSettableByOpsOnlyMode(bool topicSettableByOpsOnly);
  void setKey(const std::string &key);
  void setLimit(int limit);

  /* Member Functions */

  void removeClientFromTheChannel(int fd);
  void acceptClientInTheChannel(const Client &client);  // Utilisation du type défini
  void receiveMessageInTheChannel(int fd);
};

#endif  // INCLUDES_CHANNEL_HPP_
