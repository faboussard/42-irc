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

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include "../includes/Client.hpp"

class Channel {
 private:
  std::string                _name;
  std::string                _topic;
  std::map<int, Client>      _clients;


 public:
  explicit Channel(const std::string& name = "");

  void removeClientFromTheChannel(int fd);
  void acceptClientInTheChannel(const Client& client);
  void receiveMessageInTheChannel(int fd);

  void setTopic(const std::string& topic);
  std::string getTopic() const;
};

#endif  // INCLUDES_CHANNEL_HPP_
