/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/26 22:39:27 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

#include <cerrno>
#include <cstring>

#include "../includes/colors.hpp"

/*------ Constructors ------------------------------------------------------- */

Client::Client(int fd, const std::string& ip) : _fd(fd), _ip(ip) {}

/*------ Getters ------------------------------------------------------------ */

std::string const& Client::getNickName() const { return (_nickName); }

std::string const& Client::getUserName() const { return (_userName); }

std::string const& Client::getRealName() const { return (_realName); }

UserModes const& Client::getUserModes() const { return (_uModes); }

/*------ Setters -------------------------------------------------------------*/

void Client::setNickName(const std::string& nickName) { _nickName = nickName; }

void Client::setUserName(const std::string& userName) { _userName = userName; }

void Client::setRealName(const std::string& realName) { _realName = realName; }

void Client::setUInvisibleMode(bool isInvisible) {
  _uModes.invisible_ = isInvisible;
}

void Client::setUOperatorMode(bool isOperator) {
  _uModes.operator_ = isOperator;
}

void Client::setURegisteredMode(bool isRegistered) {
  _uModes.registered_ = isRegistered;
}

/*------ Messages handling -------------------------------------------------- */

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

std::string Client::shareMessage() {
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
