/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/17 11:53:10 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CLIENT_HPP_
#define INCLUDES_CLIENT_HPP_

#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

class Client {
private:
  int _fd;
  std::string _ip;

public:
  explicit Client(int fd = -1, const std::string &ip = "");

  void receiveMessage(const std::string &message);
  // void sendNumericReply(int code, const std::string& message);
  std::string shareMessage();

  int getFd() const { return _fd; }
  void setFd(int fd) { _fd = fd; }
  std::string getIp() const { return _ip; }
  void setIp(const std::string &ip) { _ip = ip; }
};

#endif // INCLUDES_CLIENT_HPP_
