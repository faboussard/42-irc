/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/11/26 13:02:30 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

#include <iostream>
#include <string>

#include "../includes/Bot.hpp"
#include "../includes/Parser.hpp"
#include "../includes/colors.hpp"
#include "../includes/utils.hpp"

bool Server::_signal = false;

/*============================================================================*/
/*       Constructors                                                         */
/*============================================================================*/

Server::Server(int port, const std::string &password)
    : _socketFd(-1), _port(port), _password(password), _bot(NULL) {
  _signal = false;
}

/*============================================================================*/
/*       Getters                                                              */
/*============================================================================*/

// const clientsMap &Server::getClients() const { return _clients; }

// const channelsMap &Server::getChannels() const { return _channels; }

// const std::string &Server::getPassword(void) const { return _password; }

int Server::getPort(void) const { return _port; }

// int Server::getSocketFd(void) const { return _socketFd; }

/*============================================================================*/
/*       Finders                                                              */
/*============================================================================*/

// Channel *Server::findChannelByName(const std::string &name) {
//   channelsMap::iterator it = _channels.find(name);
//   if (it == _channels.end())
//     return (NULL);
//   return (&it->second);
// }

Client *Server::findClientByNickname(const std::string &nickname) {
  clientsMap::iterator itEnd = _clients.end();
  for (clientsMap::iterator it = _clients.begin(); it != itEnd; ++it) {
    if (it->second.getNickname() == nickname) {
      return (&it->second);
    }
  }
  return (NULL);
}

/*============================================================================*/
/*       Server Mounting                                                      */
/*============================================================================*/

void Server::runServer(void) {
  createSocket();
  fetchStartTime();
  std::ostringstream oss;
  oss << "Server started on port " << _port;
  printLog(NOTIFY_LOG, SYSTEM, oss.str());
  // _bot->runBot();
  acceptAndChat();
}

void Server::createSocket(void) {
  _address.sin_family = AF_INET;
  _address.sin_addr.s_addr = INADDR_ANY;
  _address.sin_port = htons(_port);

  _socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_socketFd == -1) {
    throw std::runtime_error("Failed to create socket");
  }

  int en = 1;
  if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) {
    throw std::runtime_error("Failed to set option SO_REUSEADDR on socket");
  }

  if (fcntl(_socketFd, F_SETFL, O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set option O_NONBLOCK on socket");
  }

  if (bind(_socketFd, (struct sockaddr *)&_address, sizeof(_address)) == -1) {
    throw std::runtime_error(
        "Failed to bind socket. Port might be used elsewhere");
  }

  if (listen(_socketFd, SOMAXCONN) == -1) {
    throw std::runtime_error("Failed to listen on socket");
  }
}

void Server::fetchStartTime(void) {
  time_t now = time(0);
  char buffer[26];

  ctime_r(&now, buffer);
  std::string startTime = buffer;
  startTime.erase(startTime.find_last_not_of("\n") + 1);
  _startTime = startTime;
}

void Server::acceptAndChat(void) {
  struct pollfd newPoll;
  newPoll.fd = _socketFd;
  newPoll.events = POLLIN;
  newPoll.revents = 0;
  _pollFds.push_back(newPoll);

  // int botFdServerRead = _bot->getServerToBotPipe0();
  // int serverFdListenBot = _bot->getBotToServerPipe0();
  // addBotToPoll(botFdServerRead, serverFdListenBot);

  while (_signal == false) {
    int pollResult = poll(&_pollFds[0], _pollFds.size(), -1);
    if (pollResult == -1 && _signal == false) {
      printLog(ERROR_LOG, SYSTEM, "Error while polling");
      break;
    }
    for (size_t i = 0; i < _pollFds.size(); ++i) {
      if (_pollFds[i].revents & POLLIN && _signal == false) {
        if (_pollFds[i].fd == _socketFd) {
          acceptNewClient();
        // } else if (_pollFds[i].fd == botFdServerRead) {
        //   printLog(INFO_LOG, BOT_L, "Request has received by Bot");
        //   _bot->handleRequest();
        // } else if (_pollFds[i].fd == serverFdListenBot) {
        //   printLog(INFO_LOG, BOT_L,
        //            "Response to request has received by Server");
        //   handleBotResponse(serverFdListenBot);
        // } else if (std::find(_botToApiSocketFds.begin(),
        //                      _botToApiSocketFds.end(), _pollFds[i].fd)
        //                     != _botToApiSocketFds.end()) {
        //   // printLog(INFO_LOG, BOT_L, "Received a response from API server");
        //   _bot->handleApiResponse(_pollFds[i].fd);
        } else {
          handleClientMessage(_pollFds[i].fd);
        }
      }
    }
  }
}

void Server::signalHandler(int signal) {
  if (signal == SIGINT || signal == SIGQUIT) {
    _signal = true;
    std::string message;
    if (signal == SIGINT)
      message = "SIGINT Received";
    else
      message = "SIGQUIT Received";
    printLog(NOTIFY_LOG, SIGNAL, message);
  }
}

/*============================================================================*/
/*       Server Shutdown                                                      */
/*============================================================================*/

void Server::closeServer(void) {
  clientsMap::iterator itEnd = _clients.end();
  for (clientsMap::iterator it = _clients.begin(); it != itEnd; ++it) {
    closeClient(it->second.getFd());
  }
  _clients.clear();
  _channels.clear();
  if (_socketFd != -1) {
    std::ostringstream oss;
    oss << "fd" << _socketFd << ": Server disconnected";
    printLog(NOTIFY_LOG, SYSTEM, oss.str());
    close(_socketFd);
    _socketFd = -1;
  }
  _pollFds.clear();
  shrink_to_fit(&_pollFds);
  _channels.clear();
  delete gConfig;
  delete _bot;
}

/*============================================================================*/
/*       Clients connection Management                                        */
/*============================================================================*/

void Server::acceptNewClient(void) {
  struct sockaddr_in cliadd;
  socklen_t len = sizeof(cliadd);
  struct pollfd newPoll;

  int newClientFd =
      accept(_socketFd, reinterpret_cast<sockaddr *>(&cliadd), &len);
  if (newClientFd == -1) {
    printLog(ERROR_LOG, SYSTEM, "Failed to accept new client");
    return;
  }
  if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) == -1) {
    printLog(ERROR_LOG, SYSTEM, "fcntl() failed");
    return;
  }

  newPoll.fd = newClientFd;
  newPoll.events = POLLIN;
  newPoll.revents = 0;

  std::string clientIp = inet_ntoa(cliadd.sin_addr);
  struct hostent *host =
      gethostbyaddr(&cliadd.sin_addr, sizeof(cliadd.sin_addr), AF_INET);
  std::string hostName;
  if (host->h_name == NULL || sizeof(host->h_name) == 0 ||
      static_cast<size_t>(host->h_length) > gConfig->getLimit(HOSTLEN))
    hostName = clientIp;
  else
    hostName = host->h_name;
  Client cli(newClientFd, clientIp, hostName);
  _clients[newClientFd] = cli;
  _pollFds.push_back(newPoll);

  std::ostringstream oss;
  oss << "fd" << newClientFd << ": Listening a new client from " << clientIp
      << ". Waiting for authentification.";
  printLog(NOTIFY_LOG, SYSTEM, oss.str());
}

void Server::closeClient(int fd) {
  if (fd != -1) {
    close(fd);
    std::ostringstream oss;
    oss << "fd" << fd << ": Client disconnected";
    printLog(NOTIFY_LOG, SYSTEM, oss.str());
  }
}

void Server::clearClient(int fd) {
  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it) {
    if (it->second.getInvitedClients().find(fd) !=
        it->second.getInvitedClients().end())
      it->second.removeClientFromInvitedMap(&_clients.at(fd));
    if (it->second.getChannelOperators().find(fd) !=
        it->second.getChannelOperators().end())
      it->second.removeOperator(&_clients.at(fd));
    // if (it->second.getChannelClients().find(fd) !=
    //     it->second.getChannelClients().end())
    it->second.checkAndremoveClientFromTheChannel(fd);
  }
  closeClient(fd);
  for (size_t i = 0; i < _pollFds.size(); i++) {
    if (_pollFds[i].fd == fd) {
      _pollFds.erase(_pollFds.begin() + i);
      break;
    }
  }
  _clients.erase(fd);
}

void Server::sendConnectionMessage(const Client &client) const {
  std::string nick = client.getNickname();
  std::string user = client.getUserName();
  std::string host = client.getHostName();
  int fd = client.getFd();
  sendWelcome(fd, nick);
  send001Welcome(fd, nick, user, host);
  send002Yourhost(fd, nick);
  send003Created(fd, nick, _startTime);
  send104Myinfo(fd, nick);
  send005Isupport(fd, nick);
}

/*============================================================================*/
/*       Broadcast & send message                                             */
/*============================================================================*/

void Server::broadcastInChannel(const Client &sender, const Channel &channel,
                                const std::string &command,
                                const std::string &content) {
  std::string message = ":" + sender.getNickname() + " " + command + " " +
                        channel.getNameWithPrefix() + " :" + content + "\r\n";
  const clientPMap &allClients = channel.getChannelClients();
  clientPMap::const_iterator itEnd = allClients.end();
  for (clientPMap::const_iterator it = allClients.begin(); it != itEnd; ++it) {
    if (it->first == sender.getFd() &&
        (command == "PRIVMSG" || command == "JOIN"))
      continue;
    it->second->receiveMessage(message);
  }
}

void Server::broadcastToOperatorsOnly(const Client &sender,
                                      const Channel &channel,
                                      const std::string &command,
                                      const std::string &content) {
  std::string message = ":" + sender.getNickname() + " " + command + " " +
                        channel.getNameWithPrefix() + " :" + content + "\r\n";
  const clientPMap &operators = channel.getChannelOperators();
  clientPMap::const_iterator itEnd = operators.end();
  for (clientPMap::const_iterator it = operators.begin(); it != itEnd; ++it) {
    if (it->first != sender.getFd()) it->second->receiveMessage(message);
  }
}

void Server::sendNotice(const Client &client, const std::string &message) {
  std::ostringstream oss;
  oss << FROM_SERVER << " NOTICE " << client.getNickname() << " :" << message
      << "\r\n";
  client.receiveMessage(oss.str());
}

/*============================================================================*/
/*       Chekers                                                              */
/*============================================================================*/

bool Server::nickExists(const std::string &nick) const {
  clientsMap::const_iterator itEnd = _clients.end();
  for (clientsMap::const_iterator it = _clients.begin(); it != itEnd; ++it) {
    if (it->second.getNickname() == nick) return (true);
  }
  return (false);
}

bool Server::channelExists(const std::string &channel) {
  std::string nameToFind = std::string(channel.begin() + 1, channel.end());
  channelsMap::iterator itEnd = _channels.end();
  for (channelsMap::iterator it = _channels.begin(); it != itEnd; ++it) {
    if (it->first == nameToFind) return (true);
  }
  return (false);
}

/*============================================================================*/
/*       Server log                                                           */
/*============================================================================*/

std::string logContext(eLogContext context);

void Server::printLog(eLogLevel level, eLogContext context,
                      const std::string &message) {
  char timeStamp[20];
  time_t now = time(&now);
  struct tm tp;
  localtime_r(&now, &tp);
  std::strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d %H:%M:%S", &tp);
  std::ostringstream logHeader;
  logHeader << "[" << timeStamp << "]";

  switch (level) {
    case DEBUG_LOG:
#ifdef DEBUG
      std::cout << logHeader.str() << CYAN " DEBUG   " RESET
                << logContext(context) << message << std::endl;
      return;
#endif
    case INFO_LOG:
      logHeader << GREEN " INFO    " RESET;
      break;
    case NOTIFY_LOG:
      logHeader << BLUE << " NOTICE  " << RESET;
      break;
    case WARNING_LOG:
      logHeader << BRIGHT_YELLOW << " WARNING " << RESET;
      break;
    case ERROR_LOG:
      logHeader << RED << " ERROR   " << RESET;
      break;
    default:
      return;
  }
  if (level == INFO_LOG || level == NOTIFY_LOG)
    std::cout << logHeader.str() << logContext(context) << message << std::endl;
  else if (level == WARNING_LOG || level == ERROR_LOG)
    std::cerr << logHeader.str() << logContext(context) << message << std::endl;
}

std::string logContext(eLogContext context) {
  switch (context) {
    case SYSTEM:
      return SYSTEM_LOG;
    case SIGNAL:
      return SIGNAL_LOG;
    case PARSER:
      return PARSER_LOG;
    case AUTH:
      return AUTH_LOG;
    case COMMAND:
      return COMMAND_LOG;
    case CLIENT:
      return CLIENT_LOG;
    case CHANNEL:
      return CHANNEL_LOG;
    case REPLY:
      return REP_LOG;
    case BOT_L:
      return BOT_LOG;
  }
  return ("");
}
