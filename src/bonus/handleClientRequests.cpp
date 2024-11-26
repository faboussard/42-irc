/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleClientRequests.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:38 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/26 17:02:48 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>

#include <cerrno>
#include <string>

#include "../../includes/Bot.hpp"

#include "../../includes/Log.hpp"
#include "../../includes/Parser.hpp"

static void *handleRequestThread(void *arg);

void Bot::handleRequest(void) {
  std::string requestBuffer = readMessageFromServer();
  if (requestBuffer.empty())
    return;
  // std::stringstream ss(requestBuffer);
  // std::string commandStr;
  // std::string clientNickname;
  // std::string arg;
  // ss >> commandStr >> clientNickname;
  // std::getline(ss >> std::ws, arg);

  ThreadData *data = new ThreadData;
  pthread_t thread;
  if (pthread_create(&thread, NULL, handleRequestThread, data) != 0) {
    Log::printLog(ERROR_LOG, BOT_L,
                  "Failed to create thread for handling request: " +
                      std::string(strerror(errno)));
    delete data;
  } else {
    Log::printLog(INFO_LOG, BOT_L, "Handling a new request...");
  }
  pthread_detach(thread);
}

void *handleRequestThread(void *arg) {
  ThreadData *data = static_cast<ThreadData*>(arg);
  // Bot *bot = data->bot;
  std::string requestBuffer = data->requestBuffer;

  std::stringstream ss(requestBuffer);
  std::string commandStr;
  std::string clientNickname;
  std::string commandArg;
  ss >> commandStr >> clientNickname;
  std::getline(ss >> std::ws, commandArg);

  // std::string httpRequest = parseRequest(request);
  // sendRequestToApi
  // receiveResponseFromApi
  // parseResponse
  // sendResponseToServer
  delete data;
  return (NULL);
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


/*============================================================================*/
/*       Send to API                                                          */
/*============================================================================*/

void Bot::sendRequestToApi(const std::string &request, int socketFd) {
  ssize_t bytesSent = send(socketFd, request.c_str(), request.size(), 0);
  if (bytesSent == -1) {
    Log::printLog(ERROR_LOG, BOT_L,
                     "Failed to send request to API server: " +
                         std::string(strerror(errno)));
  } else {
    Log::printLog(INFO_LOG, BOT_L, "Sent request to API server");
    Log::printLog(DEBUG_LOG, BOT_L, "Request sent: " + request);
  }
}
