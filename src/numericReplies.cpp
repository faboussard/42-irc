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

/*============================================================================*/
/*       Welcome messages                                                     */
/*============================================================================*/
void send001Welcome(int fd, std::string const &nick, std::string const &user,
                    std::string const &host) {
  std::string message = _001_RPL_WELCOME(nick, user, host);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send002Yourhost(int fd, const std::string &nick) {
  std::string message = _002_RPL_YOURHOST(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send003Created(int fd, const std::string &nick,
                    const std::string &startTime) {
  std::string message = _003_RPL_CREATED(nick, startTime);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send104Myinfo(int fd, const std::string &nick) {
  std::string message = _004_RPL_MYINFO(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send005Isupport(int fd, const std::string &nick,
                     const std::string &tokens) {
  std::string message = _005_RPL_ISUPPORT(nick, tokens);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

/*============================================================================*/
/*       Error messages                                                       */
/*============================================================================*/
void send421UnknownCommand(int fd, const std::string &nick,
                           const std::string &command) {
  std::string message = _421_ERR_UNKNOWNCOMMAND(nick, command);
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
}

void send433NickAlreadyInUse(int fd, const std::string &nick) {
  std::string message = _433_ERR_NICKNAMEINUSE(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}

void send464PasswdMismatch(int fd, const std::string &nick) {
  std::string message = _464_ERR_PASSWD_MISMATCH(nick);
  if (send(fd, message.c_str(), message.size(), 0) == -1) {
    throw std::runtime_error(RUNTIME_ERROR);
  }
}
