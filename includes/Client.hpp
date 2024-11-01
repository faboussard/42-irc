/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/01 23:12:10 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CLIENT_HPP_
#define INCLUDES_CLIENT_HPP_

#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>
#include <sstream>
#include <string>

typedef struct UserModes {
  bool invisible;
  bool operatorOfServer;
  bool registered;
} _userModes;

class Client {
 private:
  int _fd;
  std::string _ip;
  std::string _hostName;
  std::string _nickname;
  std::string _userName;
  std::string _realName;
  bool _nicknameSet;
  bool _usernameSet;
  bool _realnameSet;
  bool _passwordGiven;
  bool _accepted;
  bool _CapSend;
  UserModes _uModes;

 public:
  explicit Client(int fd = -1, const std::string& ip = "");

  /* Getters */
  int getFd() const;
  std::string getIp() const;
  const std::string &getHostName() const;
  const std::string &getNickname() const;
  const std::string &getUserName() const;
  bool isNicknameSet() const;
  bool isUsernameSet() const;
  bool isRealnameSet() const;
  bool isPasswordGiven() const;
  bool isAccepted() const;
  bool isCapSend() const;

  const std::string &getRealName() const;
  const UserModes &getUserModes() const;
  const std::string getUserModesFlag() const;

  /* Setters */
  void setFd(int fd);
  void setNickname(const std::string& nickname);
  void setUserName(const std::string& username);
  void setRealName(const std::string& realname);
  void setIp(const std::string& ip);
  void setHostName(const std::string& hostname);
  void setCapSend(bool yes);

  void declareAccepted();
  void declarePasswordGiven();
  void setUInvisibleMode(bool isInvisible);
  void setUOperatorMode(bool isOperator);
  void setURegisteredMode(bool isRegistered);

  /* Messages handling */
  void receiveMessage(const std::string& message);
  std::string shareMessage();
};

#endif  // INCLUDES_CLIENT_HPP_
