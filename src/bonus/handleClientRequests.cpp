/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleClientRequests.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:38 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/27 16:29:19 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <string>

#include "../../includes/Bot.hpp"

void Bot::receiveRequestInQueue(BotRequest newRequest) {
  _requestDatas.push_back(newRequest);
  char notify = 1;
  if (write(_pipeServerToBot[1], &notify, sizeof(notify)) == -1) {
   Server::printLog(ERROR_LOG, BOT_L, "Failed to write to pipe in receiveRequestInQueue");
   return;
  }
}

void Bot::handleRequest(void) {
  // Clear the notification in pipe
  char buffer[2] = {0};
  if (read(_pipeServerToBot[0], buffer, sizeof(buffer) - 1) == -1)
  {
    Server::printLog(ERROR_LOG, BOT_L, "Failed to read from pipe");
  }

  if (_requestDatas.empty()) return;
  BotRequest &request = _requestDatas.front();
#ifdef DEBUG
  std::ostringstream oss;
  oss << "Request: " << request.clientNickname << " | " << "command "
      << commandToString(request.command) << " | "
      << "argument: " << request.arg;
  Server::printLog(DEBUG_LOG, BOT_L, oss.str());
#endif

  /* ------------ TEST ---------------*/

  if (request.command == NUMBERS) {
    std::string number = request.arg;
    if (number.empty()) number = "42";
    request.ApiString = "GET /" + number +
                        " HTTP/1.1\r\n"
                        "Host: numbersapi.com\r\n"
                        "Connection: close\r\n"
                        "\r\n";
  }
  else if (request.command == JOKE)
    request.ApiString  ="GET https://icanhazdadjoke.com/ HTTP/1.1\r\n"
           "Host: icanhazdadjoke.com\r\n"
           "User-Agent: ft_irc\r\n"
           "Accept: application/json\r\n"
           "Connection: close\r\n"
           "\r\n";

  else if (request.command == WEATHER)
    request.ApiString  ="GET https://icanhazdadjoke.com/ HTTP/1.1\r\n"
           "Host: icanhazdadjoke.com\r\n"
           "User-Agent: ft_irc\r\n"
           "Accept: application/json\r\n"
           "Connection: close\r\n"
           "\r\n";
  /* ------------ TEST ---------------*/
#ifndef DEBUG
  std::ostringstream oss;
  oss << request.ApiString;
  Server::printLog(INFO_LOG, BOT_L, oss.str());
#endif

  findApiInfo(&request);  // pair of host and port
  createSocketForApi(&request);
  connectToApiServer(&request);

  sendRequestToApi(request.ApiString, request.socketFd);
}

/*============================================================================*/
/*       Parse requests                                                       */
/*============================================================================*/

// std::string Bot::parseRequest(const BotRequest &request) {
//   (void)request;
//   // if (request.command == WEATHER)
//   // parse for weather
//   // else if (request.command == TRANSLATE)
//   // parse for translate
// }


Command Bot::selectRandomMode(BotRequest *request) {
    std::srand(std::time(NULL));
    Command modes[] = { WEATHER, ADVICE, INSULT, NUMBERS };
    Command selectedMode = modes[std::rand() % 4];
    switch (selectedMode) {
        case WEATHER:
            request->apiHost = WEATHER_HOST;
            request->apiPort = WEATHER_PORT;
            break;
        case ADVICE:
            request->apiHost = ADVICE_HOST;
            request->apiPort = ADVICE_PORT;
            break;
        case INSULT:
            request->apiHost = INSULT_HOST;
            request->apiPort = INSULT_PORT;
            break;
        case NUMBERS:
            request->apiHost = NUMBERS_HOST;
            request->apiPort = NUMBERS_PORT;
            break;
    }
    return (NUMBERS);
}

void Bot::findApiInfo(BotRequest *request) {
  switch (request->command) {
    case WEATHER:
      break;
    case ADVICE:
      request->apiHost = ADVICE_HOST;
      request->apiPort = ADVICE_PORT;
      break;
    case NUMBERS:
      request->apiHost = NUMBERS_HOST;
      request->apiPort = NUMBERS_PORT;
      break;
    case JOKE:
      request->apiHost = JOKE_HOST;
      request->apiPort = JOKE_PORT;
      break;
    case RANDOM:
      request->command = selectRandomMode(request);
      break;
    // case ***:
    default:
      break;
  }
}

/*============================================================================*/
/*       Send to API                                                          */
/*============================================================================*/

void Bot::createSocketForApi(BotRequest *request) {
  request->socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (request->socketFd == -1) {
    throw std::runtime_error("Failed to create socket: " +
                             std::string(strerror(errno)));
  }
  int en = 1;
  if (setsockopt(request->socketFd, SOL_SOCKET, SO_REUSEADDR, &en,
                 sizeof(en)) == -1) {
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket: " +
                             std::string(strerror(errno)));
  }
  if (fcntl(request->socketFd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket: " +
                             std::string(strerror(errno)));
  }
  logcreatSocketForApi(*request);
}

bool Bot::connectToApiServer(BotRequest *request) {
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;        // IPv4
  hints.ai_socktype = SOCK_STREAM;  // TCP socket

  int status = getaddrinfo(request->apiHost.c_str(),
                           toString(request->apiPort).c_str(), &hints, &res);
  if (status != 0) {
    Server::printLog(ERROR_LOG, BOT_L,
                     "getaddrinfo error: " + std::string(gai_strerror(status)));
    return (false);
  }

  if (connect(request->socketFd, res->ai_addr, res->ai_addrlen) == -1) {
    if (errno != EINPROGRESS) {
      Server::printLog(
          ERROR_LOG, BOT_L,
          "Failed to connect to API server: " + std::string(strerror(errno)));
      freeaddrinfo(res);
      return (false);
    }

    // Wait for connection to be established
    struct pollfd pollFd;
    pollFd.fd = request->socketFd;
    pollFd.events = POLLOUT;

    int pollRet = poll(&pollFd, 1, 10000);
    if (pollRet == -1) {
      Server::printLog(ERROR_LOG, BOT_L,
                       "poll error: " + std::string(strerror(errno)));
      freeaddrinfo(res);
      return (false);
    } else if (pollRet == 0) {
      Server::printLog(ERROR_LOG, BOT_L, "Connection to API server timed out");
      freeaddrinfo(res);
      return (false);
    }

    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(request->socketFd, SOL_SOCKET, SO_ERROR, &so_error, &len);
    if (so_error != 0) {
      Server::printLog(ERROR_LOG, BOT_L,
                       "Failed to connect to API server: " +
                           std::string(strerror(so_error)));
      freeaddrinfo(res);
      return (false);
    }
  }

  Server::printLog(
      INFO_LOG, BOT_L,
      "Connected to API server at fd" + toString(request->socketFd));
  freeaddrinfo(res);
  _server->addBotSocketFdToPoll(request->socketFd);

  return (true);
}

void Bot::sendRequestToApi(const std::string &request, int socketFd) {
  ssize_t bytesSent = send(socketFd, request.c_str(), request.size(), 0);
  if (bytesSent == -1) {
    Server::printLog(ERROR_LOG, BOT_L,
                     "Failed to send request to API server: " +
                         std::string(strerror(errno)));
  } else {
    Server::printLog(INFO_LOG, BOT_L, "Sent request to API server");
    Server::printLog(DEBUG_LOG, BOT_L, "Request sent: " + request);
  }
}
