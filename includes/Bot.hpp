/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:57 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/21 11:39:39 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_BOT_HPP_
#define INCLUDES_BOT_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <vector>

#define BOT_NAME "ircbot"
#define IRC_PORT 6668
#define IRC_PORT2 6669
#define API_PORT 8080
#define API_PORT2 8008
#define MAX_CLIENTS_BOT 20
#define LOCALHOST "127.0.0.1"

#define BOT_CONNECTION_FAILED_IRC "Failed to connect to IRC server"

class Server;

class Bot {
 private:
  Server *_server;

  int _botFdInServer;
  std::string _name;

  int _ircPort;
  int _ircSocketFd;
  struct sockaddr_in _ircAddress;
  struct pollfd _pollFdIrc;

  int _apiPort;
  int _apiSocketFd;
  struct sockaddr_in _apiAddress;
  struct pollfd _pollFdApi;

  std::vector<std::string> _instructions;

  /* Bot launch */
  void createSockets(void);
  void connectToIrcServer(void);
  void listenApiServer(void);

  /* HTTP requests */
  void handleRequest(void);  // receive, parse, send
  bool parseRequest(const std::string &request);
  void sendRequest(const std::string &request);

  /* Replies */
  void handleResponse(void);  // receive, parse, send
  bool parseResponse(const std::string &response);
  void sendResponse(const std::string &response);

 public:
  Bot(Server *server);
  ~Bot(void);

  void runBot(void);
  // void pollSockets(void);
  void closeBot(void);

  int getIrcSocketFd(void) const;
  int getApiSocketFd(void) const;
  const std::vector<std::string> &getInstructions(void) const;

  void setBotFdInServer(int fd);
};

#define BOT_RESPONSE_HEADER (std::string(":") + BOT_NAME + " PRIVMSG ")

#define BOT1 "  /\\_/\\\n"
#define BOT2 " ( o.o )\n"
#define BOT3 "─ U───U────────────────────────────────────────────────────────\n"
#define BOT4 "         Hello! I'm IRCbot, what can I do for you?"
#define BOT5 "────────────────────────────────────────────────────────── ♥ ──\n"
#define BOT6 "🌦️WEATHER: Ask me for a forecast, I'll bring you the skies.\n"
#define BOT7 "🌐TRANSLATE: Lost in translation? Let me handle the linguistics!\n"
#define BOT8 "🎨ASCII ART: \n"
#define BOT9 "How to use ?\n"
// "WEATHER → Get weather updates.\n"
// "TRANSLATE <text> → Translate words in a snap.\n"
// "ASCIIART <topic> → Create ASCII art magic.\n")

#endif  // INCLUDES_BOT_HPP_
