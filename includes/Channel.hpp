/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 08:30:30 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/26 23:20:41 by yusengok         ###   ########.fr       */
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

class Channel {
 private:
  std::string _name;
  // std::string _topic;
  Topic _topic;
  clientsMap _clientsInChannel;

 public:
  explicit Channel(const std::string &name = "");

  /* Getters */

  const std::string &getName() const;
  const clientsMap &getClientsInChannel() const;
  // const std::string &getTopic() const;
  const Topic &getTopic() const;

  /* Setters */

  // void setTopic(const std::string &topic);
  void setTopic(const std::string &topic, const std::string &author);

  /* Member Functions */

  void removeClientFromTheChannel(int fd);
  void acceptClientInTheChannel(Client &client);  // Utilisation du type défini
  void receiveMessageInTheChannel(int fd);
};

#endif  // INCLUDES_CHANNEL_HPP_
