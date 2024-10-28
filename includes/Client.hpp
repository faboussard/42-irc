/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/28 14:03:39 by mbernard         ###   ########.fr       */
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
  int _fd;
  std::string _ip;
  std::string _nickName;
  std::string _userName;
  std::string _realName;
  bool _nicknameSet;
  bool _usernameSet;
  bool _realnameSet;
  bool _passwordGiven;
  bool _accepted;
  short _messageCount;

 public:
  explicit Client(int fd = -1, const std::string& ip = "");

  // void sendNumericReply(int code, const std::string& message);

  /* Getters */
  int getFd() const;
  std::string getIp() const;
  std::string const& getNickName() const;
  std::string const& getUserName() const;
  short getMessageCount() const;
  bool isNicknameSet() const;
  bool isUsernameSet() const;
  bool isRealnameSet() const;
  bool isPasswordGiven() const;
  bool isAccepted() const;
  /* Setters */
  void setFd(int fd);
  void setNickname(const std::string& nickname);
  void setIp(const std::string& ip);
  void incrementMessageCount();
  void declareAccepted();
  void declarePasswordGiven();

  /* Messages handling */
  void receiveMessage(const std::string& message);
  std::string shareMessage();
};

#endif  // INCLUDES_CLIENT_HPP_
