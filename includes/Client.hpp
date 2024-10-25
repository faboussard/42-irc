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

#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>
#include <sstream>
#include <string>

class Client {
 private:
  int         _fd;
  std::string _ip;
  std::string _nickName;
  std::string _userName;
  std::string _realName;
  short         _messageCount;

 public:
  explicit Client(int fd = -1, const std::string &ip = "");

  // void sendNumericReply(int code, const std::string& message);

  /* Getters */
  int getFd() const;
  std::string getIp() const;
  std::string const& getNickName() const;
  std::string const& getUserName() const;
  short              getMessageCount() const;


  /* Setters */
  void setFd(int fd);
  void setNickname(const std::string& nickname);
  void setIp(const std::string &ip);
  void incrementMessageCount();


  /* Messages handling */
  void receiveMessage(const std::string& message);
  std::string shareMessage();


};

#endif  // INCLUDES_CLIENT_HPP_
