/* Copyright 2024 <yusengok>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 13:59:30 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/23 14:08:10 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ssize_t send(int sockfd, const void *buf, size_t len, int flags);

// general format of how numeric replies are structured when sent to the client:
// :<source> <numeric> <target> <parameters> :<message>
// source: The source of the message (server name or a user's nickname for
// direct user messages). target: nickname of the client receiving the message
// parameters: Additional parameters depending on the type of message
// message: A human readable text message (welcome message or error description)

#include "../includes/numericReplies.hpp"

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"

/* Welcome messages */
void send101Welcome(std::string const& nick, std::string const& user,
                    std::string const& host, int fd) {
  std::string message = _101_RPL_WELCOME(nick, user, host);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send102Yourhost(const std::string& nick, int fd) {
  std::string message = _102_RPL_YOURHOST(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send103Created(const std::string& nick, const std::string& startTime,
                    int fd) {
  std::string message = _103_RPL_CREATED(nick, startTime);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send104Myinfo(const std::string& nick, int fd) {
  std::string message = _104_RPL_MYINFO(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

/* Error messages */
void send433NickAlreadyInUse(const std::string& nick, int fd) {
  std::string message = _433_ERR_NICKNAMEINUSE(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send464PasswdMismatch(const std::string& nick, int fd) {
  std::string message = _464_ERR_PASSWD_MISMATCH(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}
