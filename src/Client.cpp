/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/28 14:26:50 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

#include <cerrno>
#include <cstring>

#include "../includes/colors.hpp"
#include "Client.hpp"

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Client::Client(int fd, const std::string& ip) : _fd(fd), _ip(ip) {
	_messageCount = 0;
  _nicknameSet = false;
  _usernameSet = false;
  _realnameSet = false;
  _passwordGiven = false;
  _accepted = false;
}

/*============================================================================*/
/*       Getters & Setters                                                    */
/*============================================================================*/

std::string const& Client::getNickName() const { return (_nickName); }

std::string const& Client::getUserName() const { return (_userName); }

int Client::getFd(void) const { return (_fd); }

std::string Client::getIp(void) const { return (_ip); }

void Client::setNickname(const std::string &nickname) {
  _nickName = nickname;
  _nicknameSet = true;
}

void Client::setFd(int fd) { _fd = fd; }

void Client::setIp(const std::string &ip) { _ip = ip; }
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

short Client::getMessageCount() const {
	return (_messageCount);
}

void Client::incrementMessageCount() {
	++_messageCount;
}

bool Client::isNicknameSet() const {
  return (_nicknameSet);
}

bool Client::isUsernameSet() const {
  return (_usernameSet);
}

bool Client::isRealnameSet() const {
  return (_realnameSet);
}

bool Client::isPasswordGiven() const {
  return (_passwordGiven);
}

bool Client::isAccepted() const {
  return (_accepted);
}

void Client::declareAccepted() {
  _accepted = true;
}

void Client::declarePasswordGiven() {
  _passwordGiven = true;
}