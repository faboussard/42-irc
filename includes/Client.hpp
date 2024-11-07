/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/07 18:21:03 by faboussa         ###   ########.fr       */
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

#include "../includes/Channel.hpp"

class Channel;

typedef std::map<int, Channel*> channelPMap;

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
  uint8_t _nbPassAttempts;
  int _channelsCount;
  channelPMap _clientChannels;

 public:
  explicit Client(int fd = -1, const std::string& ip = "",
                  const std::string& hostName = "");


/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

  int getFd(void) const;
  std::string getIp(void) const;
  const std::string& getHostName(void) const;
  const std::string& getNickname(void) const;
  const std::string& getUserName(void) const;
  const std::string& getRealName(void) const;
  const UserModes& getUserModes(void) const;
  const std::string getUserModesFlag(void) const;
  bool isNicknameSet(void) const;
  bool isUsernameSet(void) const;
  bool isRealnameSet(void) const;
  bool isPasswordGiven(void) const;
  bool isAccepted(void) const;
  bool isCapSend(void) const;
  uint8_t getNbPassAttempts(void) const;
  int getChannelsCount() const;
  const channelPMap &getChannels() const;


/*============================================================================*/
/*       Setters                                                              */
/*============================================================================*/
  void setFd(int fd);
  void setNickname(const std::string& nickname);
  void setUserName(const std::string& username);
  void setRealName(const std::string& realname);
  void setIp(const std::string& ip);
  void setHostName(const std::string& hostname);
  void setCapSend(bool yes);

  /*============================================================================*/
/*       Member functions                                                     */
/*============================================================================*/

  void declareAccepted(void);
  void declarePasswordGiven(void);
  void incrementNbPassAttempts(void);
  void setUInvisibleMode(bool isInvisible);
  void setUOperatorMode(bool isOperator);
  void setURegisteredMode(bool isRegistered);

  /* Messages handling */
  void receiveMessage(const std::string& message);
  std::string shareMessage();

  /* Channels handling */
  void incrementChannelsCount();
  void decrementChannelsCount();
};

#endif  // INCLUDES_CLIENT_HPP_