/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 08:30:30 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/23 08:30:48 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CHANNEL_HPP_
#define INCLUDES_CHANNEL_HPP_

#include <map>
#include <string>

#include "../includes/Client.hpp"

typedef std::map<int, Client> clientsMap;

class Channel {
 private:
  std::string _name;
  std::string _topic;
  clientsMap _clientsInChannel;

 public:
  explicit Channel(const std::string &name = "");

  /* Getters */

  const std::string &getName() const;
  const clientsMap &getClientsInChannel() const;
  const std::string &getTopic() const;

  /* Setters */

  void setTopic(const std::string &topic);

  /* Member Functions */


  void removeClientFromTheChannel(int fd);
  void acceptClientInTheChannel(const Client &client);  // Utilisation du type défini
  void receiveMessageInTheChannel(int fd);

};

#endif  // INCLUDES_CHANNEL_HPP_
