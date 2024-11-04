/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/04 09:05:59 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>

#include "../includes/Client.hpp"
#include "../includes/colors.hpp"

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Client::Client(int fd, const std::string& ip, const std::string& hostName) 
  : _fd(fd), _ip(ip), _hostName(hostName),
    _nickname("*"), _userName("*"), _realName("*"), 
    _nicknameSet(false), _usernameSet(false), _realnameSet(false),
    _passwordGiven(false), _accepted(false), _CapSend(false) {
  _uModes.invisible = false;
  _uModes.operatorOfServer = false;
  _uModes.registered = false;
}

// Client::Client(int fd, const std::string& ip) : _fd(fd), _ip(ip) {
//   _nicknameSet = false;
//   _usernameSet = false;
//   _realnameSet = false;
//   _passwordGiven = false;
//   _accepted = false;
// }

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

std::string const& Client::getNickname(void) const { return (_nickname); }

std::string const& Client::getUserName(void) const { return (_userName); }

int Client::getFd(void) const { return (_fd); }

std::string Client::getIp(void) const { return (_ip); }

std::string const& Client::getHostName(void) const { return (_hostName); }

std::string const& Client::getRealName(void) const { return (_realName); }

UserModes const& Client::getUserModes(void) const { return (_uModes); }

const std::string Client::getUserModesFlag(void) const {
  std::string flags = "+";
  if (_uModes.invisible) flags += "i";
  if (_uModes.operatorOfServer) flags += "o";
  if (_uModes.registered) flags += "r";
  return (flags);
}

bool Client::isNicknameSet(void) const { return (_nicknameSet); }

bool Client::isUsernameSet(void) const { return (_usernameSet); }

bool Client::isRealnameSet(void) const { return (_realnameSet); }

bool Client::isPasswordGiven(void) const { return (_passwordGiven); }

bool Client::isAccepted(void) const { return (_accepted); }

bool Client::isCapSend(void) const { return (_CapSend); }

/*============================================================================*/
/*       Setters                                                              */
/*============================================================================*/

void Client::setNickname(const std::string& nickname) {
  _nickname = nickname;
  _nicknameSet = true;
}

void Client::setUserName(const std::string& username) {
  _userName = username;
  _usernameSet = true;
}

void Client::setRealName(const std::string& realname) {
  _realName = realname;
  _realnameSet = true;
}

void Client::setFd(int fd) { _fd = fd; }

void Client::setIp(const std::string& ip) { _ip = ip; }

void Client::setHostName(const std::string& hostname) { _hostName = hostname; }

void Client::setCapSend(bool yes) { _CapSend = yes; }

void Client::setUInvisibleMode(bool isInvisible) {
  _uModes.invisible = isInvisible;
}

void Client::setUOperatorMode(bool isOperator) {
  _uModes.operatorOfServer = isOperator;
}

void Client::setURegisteredMode(bool isRegistered) {
  _uModes.registered = isRegistered;
}

void Client::declareAccepted(void) { _accepted = true; }

void Client::declarePasswordGiven(void) { _passwordGiven = true; }


/*============================================================================*/
/*       Messages handling                                                    */
/*============================================================================*/

void Client::receiveMessage(const std::string& message) {
  if (_fd != -1) {
    ssize_t sent = send(_fd, message.c_str(), message.length(), 0);
    if (sent == -1) {
      std::cerr << RED "Error while sending message to fd " RESET << _fd << ": "
                << strerror(errno) << RESET << std::endl;
    }
  } else {
    std::cerr << RED "Invalid file descriptor" RESET << std::endl;
  }
}

std::string Client::shareMessage(void) {
  char buffer[1024] = {0};
  ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer) - 1, 0);
  // std::cout << MAGENTA << "Received message from client " << _fd << ": " <<
  // buffer
  //           << RESET << std::endl;
  if (bytesRead == -1) {
    std::cerr << RED "Error while receiving message: " RESET << std::endl;
    return ("");
  } else if (bytesRead == 0) {
    std::cerr << RED "Connection closed by peer" RESET << std::endl;
    return ("");
  }
  buffer[bytesRead] = '\0';
  return (std::string(buffer));
}
