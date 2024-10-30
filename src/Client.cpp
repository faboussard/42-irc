/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/30 10:58:22 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstring>

#include "../includes/Client.hpp"
#include "../includes/colors.hpp"

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Client::Client(int fd, const std::string& ip) : _fd(fd), _ip(ip) {
  _nicknameSet = false;
  _usernameSet = false;
  _realnameSet = false;
  _passwordGiven = false;
  _accepted = false;
}

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

std::string const& Client::getNickName() const { return (_nickName); }

std::string const& Client::getUserName() const { return (_userName); }

int Client::getFd(void) const { return (_fd); }

std::string Client::getIp(void) const { return (_ip); }

std::string const& Client::getRealName() const { return (_realName); }

UserModes const& Client::getUserModes() const { return (_uModes); }

const std::string Client::getUserModesFlag() const {
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

/*============================================================================*/
/*       Setters                                                              */
/*============================================================================*/

void Client::setNickname(const std::string& nickname) {
  _nickName = nickname;
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
