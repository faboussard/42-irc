/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:01:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/26 17:20:19 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

#include <unistd.h>

#include <cerrno>
#include <string>
#include <vector>

#include "../../includes/Log.hpp"

bool Bot::_signal = false;

/*============================================================================*/
/*       Constructor & destructor                                             */
/*============================================================================*/

Bot::Bot(int serverPort, const std::string &serverPass, int botPort)
    : _nick(BOT_NICK),
      _user(BOT_USER),
      _serverPort(serverPort),
      _serverPass(serverPass),
      _botPort(botPort),
      _botSocketFd(-1) {
  _instructions.push_back(BOT1);
  _instructions.push_back(BOT2);
  _instructions.push_back(BOT3);
  _instructions.push_back(BOT4);
  _instructions.push_back(BOT5);
  _instructions.push_back(BOT6);
  _instructions.push_back(BOT7);
  _instructions.push_back(BOT8);
  _instructions.push_back(BOT9);
}

Bot::~Bot(void) { close(_botSocketFd); }

/*============================================================================*/
/*       Bot launch                                                           */
/*============================================================================*/

void Bot::runBot(void) {
  createSocket();
  connectToIrcServer();
  listenToIrcServer();
}

void Bot::createSocket(void) {
  _botSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_botSocketFd == -1) {
    throw std::runtime_error("Failed to create socket: " +
                             std::string(strerror(errno)));
  }
  int en = 1;
  if (setsockopt(_botSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) ==
      -1) {
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket: " +
                             std::string(strerror(errno)));
  }
  if (fcntl(_botSocketFd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket: " +
                             std::string(strerror(errno)));
  }
  logcreatSocketForApi();
}

void Bot::connectToIrcServer(void) {
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;        // IPv4
  hints.ai_socktype = SOCK_STREAM;  // TCP socket

  int status =
      getaddrinfo(LOCALHOST, toString(_serverPort).c_str(), &hints, &res);
  if (status != 0)
    throw std::runtime_error("getaddrinfo error: " +
                             std::string(gai_strerror(status)));
  if (connect(_botSocketFd, res->ai_addr, res->ai_addrlen) == -1) {
    if (errno != EINPROGRESS)
      throw std::runtime_error("Failed to connect to API server: " +
                               std::string(strerror(errno)));

    // Wait for connection to be established
    struct pollfd pollFd;
    pollFd.fd = _botSocketFd;
    pollFd.events = POLLOUT;

    int pollRet = poll(&pollFd, 1, 10000);
    if (pollRet == -1)
      throw std::runtime_error("Poll error: " + std::string(strerror(errno)));
    else if (pollRet == 0) {
      throw std::runtime_error("Connection to IRC server timed out");

      int so_error;
      socklen_t len = sizeof(so_error);
      getsockopt(_botSocketFd, SOL_SOCKET, SO_ERROR, &so_error, &len);
      if (so_error != 0)
        throw std::runtime_error("Failed to connect to IRC server: " +
                                 std::string(strerror(so_error)));
    }
  }
  Log::printLog(INFO_LOG, BOT_L,
                "Connected to IRC server at fd" + toString(_botSocketFd));
  freeaddrinfo(res);
  if (!authenticate())
    throw std::runtime_error("Failed to authenticate to IRC server");
}

void Bot::listenToIrcServer(void) {
  struct pollfd botPolls[1];
  botPolls[0].fd = _botSocketFd;
  botPolls[0].events = POLLIN;

  while (_signal == false) {
    int pollRet = poll(botPolls, 1, -1);
    if (pollRet == -1 && _signal == false)
      throw std::runtime_error("Poll error: " + std::string(strerror(errno)));
    if (botPolls[0].revents & POLLIN && _signal == false) handleRequest();
  }
}

bool Bot::authenticate(void) {
  if (!sendMessageToServer("PASS " + _serverPass + "\r\n") ||
      !sendMessageToServer("NICK " + _nick + "\r\n") ||
      !sendMessageToServer("USER " + _user + "\r\n")) {
    Log::printLog(ERROR_LOG, BOT_L,
                  "Failed to send authentication message to Server");
    return (false);
  }
  if (!sendMessageToServer("PING :ft_irc\r\n")) {
    Log::printLog(ERROR_LOG, BOT_L, "Failed to send PING message to Server");
    return (false);
  }
  // std::string response = readMessageFromServer();
  // if (response != "PONG :ft_irc\r\n") {
  //   Log::printLog(ERROR_LOG, BOT_L, "Failed to authenticate to IRC server");
  //   return (false);
  // }
  return (true);
}

void Bot::signalHandler(int signal) {
  if (signal == SIGINT || signal == SIGQUIT) {
    _signal = true;
    std::string message;
    if (signal == SIGINT)
      message = "SIGINT Received";
    else
      message = "SIGQUIT Received";
    Log::printLog(NOTIFY_LOG, SIGNAL, message);
  }
}

/*============================================================================*/
/*       Communicate with Server                                              */
/*============================================================================*/

std::string Bot::readMessageFromServer(void) {
  std::string newMessage = "";
  char buffer[1024] = {0};
  std::memset(buffer, 0, sizeof(buffer));
  int valread = recv(_botSocketFd, buffer, sizeof(buffer), 0);
  if (valread == -1)
    throw std::runtime_error("Failed to receive message from IRC server");
  // if (valread == 0)
  //   return;
  buffer[valread] = '\0';
  newMessage += std::string(buffer, valread);
  return (newMessage);
}

// static std::map<int, std::string> messageBuffer;
// char buffer[1024] = {0};
// std::memset(buffer, 0, sizeof(buffer));
// int valread = recv(fd, buffer, sizeof(buffer), 0);

// if (valread == -1) {
//   std::ostringstream oss;
//   oss << "fd" << fd << ": Error occurred while receiving a message.";
//   Server::printLog(ERROR_LOG, CLIENT, oss.str());
//   return;
// }

bool Bot::sendMessageToServer(const std::string &message) {
  ssize_t bytesSent = send(_botSocketFd, message.c_str(), message.length(), 0);
  if (bytesSent == -1) {
    Log::printLog(ERROR_LOG, BOT_L,
                  "Failed to send message to IRC server: " +
                      std::string(strerror(errno)));
    return (false);
  }
  return (true);
}
