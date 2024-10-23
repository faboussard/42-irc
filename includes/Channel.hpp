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

#include <string>
#include <map>

#include "Client.hpp"

class Client; 

typedef std::map<int, Client> clientsMap;
typedef Client& ClientRef; 

class Channel {
 private:
  std::string _name;
  std::string _topic;
  clientsMap _clients; 

 public:
  explicit Channel(const std::string& name = "");

  void removeClientFromTheChannel(int fd);
  void acceptClientInTheChannel(ClientRef client);  // Utilisation du type défini
  void receiveMessageInTheChannel(int fd);

  void setTopic(const std::string& topic);
  std::string getTopic() const;
};

#endif  // INCLUDES_CHANNEL_HPP_
