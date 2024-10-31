/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/31 14:08:00 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CLIENT_HPP_
#define INCLUDES_CLIENT_HPP_

#include <stdint.h>
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
  std::string _nickName;
  std::string _userName;
  std::string _realName;
  bool _nicknameSet;
  bool _usernameSet;
  bool _realnameSet;
  bool _passwordGiven;
  bool _accepted;
  bool _CapSend;
  UserModes _uModes;
  uint8_t _nbPassAttempts;

 public:
  explicit Client(int fd = -1, const std::string& ip = "");

  // void sendNumericReply(int code, const std::string& message);

  /* Getters */
  int getFd(void) const;
  std::string getIp(void) const;
  const std::string& getNickName(void) const;
  const std::string& getUserName(void) const;
  bool isNicknameSet(void) const;
  bool isUsernameSet(void) const;
  bool isRealnameSet(void) const;
  bool isPasswordGiven(void) const;
  bool isAccepted(void) const;
  bool isCapSend(void) const;

  const std::string& getRealName(void) const;
  const UserModes& getUserModes(void) const;
  const std::string getUserModesFlag(void) const;
  uint8_t getNbPassAttempts(void) const;

  /* Setters */
  void setFd(int fd);
  void setNickname(const std::string& nickname);
  void setUserName(const std::string& username);
  void setRealName(const std::string& realname);
  void setIp(const std::string& ip);
  void setCapSend(bool yes);

  void declareAccepted(void);
  void declarePasswordGiven(void);
  void incrementNbPassAttempts(void);
  void setUInvisibleMode(bool isInvisible);
  void setUOperatorMode(bool isOperator);
  void setURegisteredMode(bool isRegistered);

  /* Messages handling */
  void receiveMessage(const std::string& message);
  std::string shareMessage(void);
};

#endif  // INCLUDES_CLIENT_HPP_
