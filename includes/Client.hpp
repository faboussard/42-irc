/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/26 08:52:48 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_CLIENT_HPP_
#define INCLUDES_CLIENT_HPP_

#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>
#include <map>
#include <sstream>
#include <string>

class Channel;

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
  uint8_t _nbPassAttempts;
  uint8_t _channelsCount;

  /* bonus */
  bool _botLaunched;

 public:
  explicit Client(int fd = -1, const std::string& ip = "",
                  const std::string& hostName = "");

  /* Getters */
  int getFd(void) const;
  std::string getIp(void) const;
  const std::string& getHostName(void) const;
  const std::string& getNickname(void) const;
  const std::string& getUserName(void) const;
  const std::string& getRealName(void) const;
  bool isNicknameSet(void) const;
  bool isUsernameSet(void) const;
  bool isRealnameSet(void) const;
  bool isPasswordGiven(void) const;
  bool isAccepted(void) const;
  uint8_t getNbPassAttempts(void) const;
  uint8_t getChannelsCount() const;
  bool botLaunched(void) const;  // bonus

  /* Setters */
  void setFd(int fd);
  void setNickname(const std::string& nickname);
  void setUserName(const std::string& username);
  void setRealName(const std::string& realname);
  void setIp(const std::string& ip);
  void setHostName(const std::string& hostname);
  void setBotLaunched(bool lanched);  // bonus

  /* Client status */
  void declareAccepted(void);
  void declarePasswordGiven(void);
  void incrementNbPassAttempts(void);

  /* Messages handling */
  void receiveMessage(const std::string& message) const;
  std::string shareMessage();

  /* Channels handling */
  void incrementChannelsCount();
  void decrementChannelsCount();
};

#endif  // INCLUDES_CLIENT_HPP_
