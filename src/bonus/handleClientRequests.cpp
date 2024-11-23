/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleClientRequests.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:38 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/23 18:07:57 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

void Bot::receiveRequestInQueue(const std::string &request) {
  _requests.push(request);
  char notify = 1;
  write(_pipeServerToBot[1], &notify, sizeof(notify));
}

void Bot::handleRequest(void) {
  // Clear the notification in pipe
  char buffer[2] = {0};
  read(_pipeServerToBot[0], buffer, sizeof(buffer) - 1);

  BotRequest request = readRequest();

  // std::string httpRequest = parseRequest(request);
  // if (httpRequest.empty())
  // send error message to server that sends it to client
  // return;

  // sendRequestToApi(httpRequest);

/* --------- TEST -------------*/
  // std::string apiHost = "v2.jokeapi.dev";
  std::string apiHost = "numbersapi.com";
  int apiPort = 80;
  createSocketForApi(apiHost, apiPort);
  connectToApiServer(apiHost, apiPort, _socketFds[0]);  // _socketFds[0] is temporary

  std::ostringstream oss;
//   oss << "GET /joke/Any?blacklistFlags=religious,racist,sexist&type=single HTTP/2\r\n"
//         "Host: v2.jokeapi.dev\r\n"
//         "Connection: close\r\n"
//         "\r\n";
  oss << "GET /42 HTTP/1.1\r\n"
        "Host: numbersapi.com\r\n"
        "Connection: close\r\n"
        "\r\n";
  std::string requestStr = oss.str();
  sendRequestToApi(requestStr, _socketFds[0]);  // _socketFds[0] is temporary
}

/*============================================================================*/
/*       Read & Parse requests                                                */
/*============================================================================*/

BotRequest Bot::readRequest(void) {
  if (!_requests.empty()) {
    std::stringstream ss(_requests.front());
    _requests.pop();
    std::string clientNickname;
    std::string command;
    std::string arg;
    ss >> clientNickname;
    ss >> command;
    std::getline(ss >> std::ws, arg);
    BotRequest request(clientNickname, command, arg);
#ifdef DEBUG
    debugLogReadRequest(request);
#endif
    return (request);
  }
  return (BotRequest("", "", ""));
}

// std::string Bot::parseRequest(const BotRequest &request) {
//   (void)request;
//   // if (request.command == "WEATHER")
//   // parse for weather
//   // else if (request.command == "TRANSLATE")
//   // parse for translate
// }

/*============================================================================*/
/*       Send to API                                                          */
/*============================================================================*/

void Bot::createSocketForApi(const std::string &host, int port) {
  int socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFd == -1) {
    throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
  }
  int en = 1;
  if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) {
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket: " + std::string(strerror(errno)));
  }
  if (fcntl(socketFd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket: " + std::string(strerror(errno)));
  }
  _socketFds.push_back(socketFd);
  logcreatSocketForApi(socketFd, host, port);
}

bool Bot::connectToApiServer(const std::string &host, int port, int socketFd) {
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;  // IPv4
  hints.ai_socktype = SOCK_STREAM;  // TCP socket

  int status = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res);
  if (status != 0) {
    Server::printLog(ERROR_LOG, BOT_L, "getaddrinfo error: " + std::string(gai_strerror(status)));
    return (false);
  }

  if (connect(socketFd, res->ai_addr, res->ai_addrlen) == -1) {
    if (errno != EINPROGRESS) {
      Server::printLog(ERROR_LOG, BOT_L, "Failed to connect to API server: " + std::string(strerror(errno)));
      freeaddrinfo(res);
      return (false);
    }

    // Wait for connection to be established
    struct pollfd pollFd;
    pollFd.fd = socketFd;
    pollFd.events = POLLOUT;

    int pollRet = poll(&pollFd, 1, 10000);
    if (pollRet == -1) {
      Server::printLog(ERROR_LOG, BOT_L, "poll error: " + std::string(strerror(errno)));
      freeaddrinfo(res);
      return (false);
    } else if (pollRet == 0) {
      Server::printLog(ERROR_LOG, BOT_L, "Connection to API server timed out");
      freeaddrinfo(res);
      return (false);
    }

    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &so_error, &len);
    if (so_error != 0) {
      Server::printLog(ERROR_LOG, BOT_L, "Failed to connect to API server: " + std::string(strerror(so_error)));
      freeaddrinfo(res);
      return (false);
    }
  }

  Server::printLog(INFO_LOG, BOT_L, "Connected to API server at fd" + toString(socketFd)); ;
  freeaddrinfo(res);
  _server->addBotApiSocketFdToPoll(socketFd);

  return (true);
}

void Bot::sendRequestToApi(const std::string &request, int socketFd) {
  ssize_t bytesSent = send(socketFd, request.c_str(), request.size(), 0);
  if (bytesSent == -1) {
    Server::printLog(ERROR_LOG, BOT_L, "Failed to send request to API server: " + std::string(strerror(errno)));
  } else {
    Server::printLog(INFO_LOG, BOT_L, "Sent request to API server");
    Server::printLog(DEBUG_LOG, BOT_L, "Request sent: " + request);
  }
}

/*============================================================================*/
/*       Logs                                                                 */
/*============================================================================*/

void logcreatSocketForApi(int fd, const std::string &host, int port) {
  std::ostringstream oss;
  oss << "fd" << fd << ": Ready to communicate with API Server " << host << ":"
      << port;
  Server::printLog(INFO_LOG, BOT_L, oss.str());
}

#ifdef DEBUG
void debugLogReadRequest(BotRequest request) {
  std::ostringstream oss;
  oss << "New request from " CYAN << request.clientNickname
      << RESET ": Command " << CYAN << request.command
      << RESET " | " << "arg " << CYAN << request.arg << RESET;
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
}
#endif
