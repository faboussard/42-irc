/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:01:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/30 20:48:23 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

#include <unistd.h>

#include <cerrno>
#include <string>
#include <vector>

#include "../../includes/asciiCats.hpp"
#include "../../includes/Log.hpp"

/*============================================================================*/
/*       Initialization / Constructor / Destructor                            */
/*============================================================================*/

bool Bot::_signal = false;

Bot::Bot(int serverPort, const std::string &serverPass)
    : _nick(BOT_NICK),
      _user(BOT_USER),
      _serverPort(serverPort),
      _serverPass(serverPass),
      _botSocketFd(-1) {
  constructInstruction();
  constructAsciiCats();
}

Bot::~Bot(void) { close(_botSocketFd); }

void Bot::constructInstruction(void) {
  const char* menus[] = BOT_MENU;
  _instructions.assign(menus, menus + sizeof(menus) / sizeof(menus[0]));
}

void Bot::constructAsciiCats(void) {
  const char* jokeCat[] = JOKE_CAT;
  _jokeCat.assign(jokeCat, jokeCat + sizeof(jokeCat) / sizeof(jokeCat[0])); 

  const char* adviceCat[] = ADVICE_CAT;
  _adviceCat.assign(adviceCat, adviceCat + sizeof(adviceCat) / sizeof(adviceCat[0]));

  const char* insultMeCat[] = INSULTME_CAT;
  _insultMeCat.assign(insultMeCat, insultMeCat + sizeof(insultMeCat) / sizeof(insultMeCat[0]));

  const char* sunnyCat[] = SUNNY_CAT;
  _sunnyCat.assign(sunnyCat, sunnyCat + sizeof(sunnyCat) / sizeof(sunnyCat[0]));
  const char* cloudyCat[] = CLOUDY_CAT;
  _cloudyCat.assign(cloudyCat, cloudyCat + sizeof(cloudyCat) / sizeof(cloudyCat[0]));
  const char* foggyCat[] = FOGGY_CAT;
  _foggyCat.assign(foggyCat, foggyCat + sizeof(foggyCat) / sizeof(foggyCat[0]));
  const char* rainyCat[] = RAINY_CAT;
  _rainyCat.assign(rainyCat, rainyCat + sizeof(rainyCat) / sizeof(rainyCat[0]));
  const char* snowyCat[] = SNOWY_CAT;
  _snowyCat.assign(snowyCat, snowyCat + sizeof(snowyCat) / sizeof(snowyCat[0]));
  const char* thunderCat[] = THUNDER_CAT;
  _thunderCat.assign(thunderCat, thunderCat + sizeof(thunderCat) / sizeof(thunderCat[0]));
  const char* frostyCat[] = FROSTY_CAT;
  _frostyCat.assign(frostyCat, frostyCat + sizeof(frostyCat) / sizeof(frostyCat[0]));

  const char* unknownCat[] = DEFAULT_CAT;
  _unknownCat.assign(unknownCat, unknownCat + sizeof(unknownCat) / sizeof(unknownCat[0]));

  const char* timeoutCat[] = TIMEOUT_CAT;
  _timeoutCat.assign(timeoutCat, timeoutCat + sizeof(timeoutCat) / sizeof(timeoutCat[0]));
}

/*============================================================================*/
/*       Bot launch                                                           */
/*============================================================================*/

void Bot::runBot(void) {
  createSocket();
  connectToIrcServer();
  listenToIrcServer();
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

/*============================================================================*/
/*       Connection to Server                                                 */
/*============================================================================*/

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
    if (pollRet == -1) {
      throw std::runtime_error("Poll error: " + std::string(strerror(errno)));
    } else if (pollRet == 0) {
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
                "Connected to IRC server at fd " + toString(_botSocketFd));
  freeaddrinfo(res);
  if (!authenticate())
    throw std::runtime_error("");
}

bool Bot::authenticate(void) {
  if (!sendMessageToServer("PASS " + _serverPass + "\r\n")) {
    Log::printLog(ERROR_LOG, BOT_L, "Failed to authenticate to IRC server");
    return (false);
  }
  if (!waitForPassAuthentication(2000))
    return (false);
  if (!sendMessageToServer("NICK " + _nick + "\r\n")) {
    Log::printLog(ERROR_LOG, BOT_L, "Failed to authenticate to IRC server");
    return (false);
  }
  if (!waitForNickValidation(2000))
    return (false);
  if (!sendMessageToServer("USER " + _user + "\r\n")) {
    Log::printLog(ERROR_LOG, BOT_L, "Failed to authenticate to IRC server");
    return (false);
  }
  if (!waitForConnectionMessage(5000))
    return (false);
  return (true);
}

bool Bot::waitForPassAuthentication(int timeLimitInMs) {
  Log::printLog(INFO_LOG, BOT_L, "Waiting for password authentication");
  struct pollfd fd;
  fd.fd = _botSocketFd;
  fd.events = POLLIN;
  int pollRet = poll(&fd, 1, timeLimitInMs);
  if (pollRet == -1) {
    Log::printLog(ERROR_LOG, BOT_L,
                  "Failed to poll for server response: " +
                      std::string(strerror(errno)));
    return (false);
  } if (pollRet == 0) {
    Log::printLog(INFO_LOG, BOT_L, "Password authentication successful");
    return (true);
  } else {
    std::string message = readMessageFromServer();
    if (message.find("464") != std::string::npos) {
      Log::printLog(ERROR_LOG, BOT_L,
                  "It seems I don't have a good key to IRC server");
      return (false);
    }
  }
  Log::printLog(INFO_LOG, BOT_L, "Password authentication successful");
  return (true);
}

bool Bot::waitForNickValidation(int timeLimitInMs) {
  Log::printLog(INFO_LOG, BOT_L, "Waiting for nickname and username validation");
  struct pollfd fd;
  fd.fd = _botSocketFd;
  fd.events = POLLIN;
  int pollRet = poll(&fd, 1, timeLimitInMs);
  if (pollRet == -1) {
    Log::printLog(ERROR_LOG, BOT_L,
                  "Failed to poll for server response: " +
                      std::string(strerror(errno)));
    return (false);    
  }
  if (pollRet && fd.revents & POLLIN) {
    std::string message = readMessageFromServer();
    if (message.find("433") != std::string::npos) {
      Log::printLog(ERROR_LOG, BOT_L,
                  "Someone has stolen my nickname. I can't connect to IRC server. Bye.");
      return (false);
    }
  }
  Log::printLog(INFO_LOG, BOT_L, "Nickname validation successful");
  return (true);
}

bool Bot::waitForConnectionMessage(int timeLimitInMs) {
  struct pollfd fd;
  fd.fd = _botSocketFd;
  fd.events = POLLIN;
  int totalWaitTime = 0;
  const int pollInterval = 100;

  while (totalWaitTime < timeLimitInMs) {
    int pollRet = poll(&fd, 1, pollInterval);
    if (pollRet == -1) {
      Log::printLog(ERROR_LOG, BOT_L,
                    "Failed to poll for server response: " +
                    std::string(strerror(errno)));
      return (false);  
    }
    if (pollRet > 0 && (fd.revents & POLLIN)) {
      std::string message = readMessageFromServer();
      if (message.find("005") != std::string::npos) {
        Log::printLog(INFO_LOG, BOT_L, "Registration to IRC server successful");
        return true;
      }
    }
    totalWaitTime += pollInterval;
  }
  Log::printLog(ERROR_LOG, BOT_L, "Registration attempt timeout");
  return (false);
}

/*============================================================================*/
/*       Listen to Server                                                     */
/*============================================================================*/

void Bot::listenToIrcServer(void) {
  struct pollfd newPoll;
  newPoll.fd = _botSocketFd;
  newPoll.events = POLLIN;
  newPoll.revents = 0;
  _botPollFds.push_back(newPoll);

  while (_signal == false) {
    int pollRet = poll(&_botPollFds[0], _botPollFds.size(), 100);
    if (pollRet == -1 && _signal == false) {
      throw std::runtime_error("Poll error: " + std::string(strerror(errno)));
    }
    size_t botPollFdsSize = _botPollFds.size();
    for (size_t i = 0; i < botPollFdsSize; ++i) {
      if (_botPollFds[i].revents & POLLIN && _signal == false) {
        if (_botPollFds[i].fd == _botSocketFd) {
          handleServerMessage();
        } else {
          handleApiResponse(_botPollFds[i].fd);
        }
      }
    }

    // Check for API requests timeout
    for (std::deque<BotRequest>::iterator it = _requestDatas.begin(); it != _requestDatas.end(); ) {
      if (it->timeoutInMs > 0) {
        it->timeoutInMs -= 100;
        ++it;
      } else {
        close(it->fdForApi);
        pclose(it->fpForApi);
        std::vector<struct pollfd>::iterator pollItEnd = _botPollFds.end();
        for (std::vector<struct pollfd>::iterator pollIt = _botPollFds.begin(); pollIt != pollItEnd; ++pollIt) {
          if (pollIt->fd == it->fdForApi) {
            _botPollFds.erase(pollIt);
            break;
          }
        }
        sendAsciiCatServiceUnavailable(&(*it));
        logApiTimeout(it->fdForApi, it->command);
        it = _requestDatas.erase(it);
      }
    }
  }
}

/*============================================================================*/
/*       Communicate with Server                                              */
/*============================================================================*/

std::string Bot::readMessageFromServer(void) {
  std::string newMessage = "";
  char buffer[512] = {0};
  std::memset(buffer, 0, sizeof(buffer));
  int valread = recv(_botSocketFd, buffer, sizeof(buffer), 0);
  if (valread == -1) {
    Log::printLog(DEBUG_LOG, BOT_L, "There is no message received from IRC server.");
    return ("");
  }
  if (valread == 0) return ("");
  buffer[valread] = '\0';
  newMessage += std::string(buffer, valread);
  return (newMessage);
}

bool Bot::sendMessageToServer(const std::string &message) {
  ssize_t bytesSent = send(_botSocketFd, message.c_str(), message.length(), 0);
  if (bytesSent == -1) {
    Log::printLog(ERROR_LOG, BOT_L,
                  "Failed to send message to IRC server: " +
                      std::string(strerror(errno)));
    return (false);
  }
  Log::printLog(INFO_LOG, BOT_L, "Message sent to IRC server: " + message);
  return (true);
}
