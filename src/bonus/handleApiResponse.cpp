/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleApiResponse.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 14:59:45 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/02 15:09:28 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <string>
#include <vector>

#include "../../includes/Bot.hpp"
#include "../../includes/Log.hpp"
#include "../../includes/enums.hpp"

/*============================================================================*/
/*       API responses handling                                               */
/*============================================================================*/

void Bot::handleApiResponse(int fd) {
  logApiResponse(fd);
  std::deque<BotRequest>::iterator it = _requestDatas.begin();
  std::deque<BotRequest>::iterator itEnd = _requestDatas.end();
  for (; it != itEnd; ++it) {
    if (it->fdForApi == fd) {
      break;
    }
  }
  receiveResponseFromApi(it);
  sendResponseToServer(it);

  std::vector<struct pollfd>::iterator itPollEnd = _botPollFds.end();
  for (std::vector<struct pollfd>::iterator itPoll = _botPollFds.begin();
       itPoll != itPollEnd; ++itPoll) {
    if (itPoll->fd == fd) {
      _botPollFds.erase(itPoll);
      break;
    }
  }
  _requestDatas.erase(it);
  Log::printLog(INFO_LOG, BOT_L,
                "Request removed from queue. Remaining requests count: " +
                    toString(_requestDatas.size()));
}

void Bot::receiveResponseFromApi(std::deque<BotRequest>::iterator itRequest) {
  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), itRequest->fpForApi) != NULL) {
    itRequest->apiResponse += buffer;
  }
  pclose(itRequest->fpForApi);
  Log::printLog(INFO_LOG, BOT_L,
                "API response received: " + itRequest->apiResponse);
#ifdef DEBUG
  debugLogWaitingRequests();
#endif
}

void Bot::sendResponseToServer(std::deque<BotRequest>::iterator itRequest) {
  std::string content;
  if (itRequest->command == ADVICE) {
    content = parseResponseByKey(itRequest->apiResponse, "advice");
  } else if (itRequest->command != WEATHER) {
    content = itRequest->apiResponse;
  } else {
    eForecast forecast = parseJsonWeatherResponse(itRequest->apiResponse);
    if (forecast != UNKNOWN_FORECAST) {
      std::string city =
          itRequest->commandArg.empty() ? DEFAULT_CITY : itRequest->commandArg;
      capitalize(&city);
      std::ostringstream oss;
      oss << "PRIVMSG " << itRequest->clientNickname << " :Tomorrow in " << city
          << "... " << "\r\n";
      sendMessageToServer(oss.str());
    }
    sendAsciiCatForecast(itRequest->clientNickname, forecast);
    return;
  }
#ifdef DEBUG
  std::ostringstream oss;
  oss << "Response to send: " << content;
  Log::printLog(DEBUG_LOG, BOT_L, oss.str());
#endif
  if (content.empty()) {
    Log::printLog(ERROR_LOG, BOT_L, "Response is empty");
    return;
  }
  std::string response =
      "PRIVMSG " + itRequest->clientNickname + " :" + content + "\r\n";
  sendMessageToServer(response);
  sendAsciiCatByCommand(&(*itRequest), itRequest->command);
  Log::printLog(INFO_LOG, BOT_L, "Response has to be sent to Server");
}

/*============================================================================*/
/*       Responses parsing                                                    */
/*============================================================================*/

static std::string extractTomorrowForecastCode(const std::string &apiResponse);
static eForecast decodeWeatherCode(const std::string &code);
static bool isCodeSnowy(const std::string &code);

std::string Bot::parseResponseByKey(const std::string &response,
                                    const std::string &key) {
  std::string keyPattern = "\"" + key + "\": \"";
  std::size_t start = response.find(keyPattern);
  if (start == std::string::npos) {
    return ("I cannot find any advice for you !");
  }
  start += keyPattern.length();
  std::size_t end = response.find("\"", start);
  if (end == std::string::npos) {
    return ("I cannot find any advice for you !");
  }
  return response.substr(start, end - start);
}

eForecast Bot::parseJsonWeatherResponse(const std::string &apiResponse) {
  // "error":{"code":1006 (No matching location found.)
  if (apiResponse.empty()) return (UNKNOWN_FORECAST);
  std::string weatherCode = extractTomorrowForecastCode(apiResponse);
  if (weatherCode.empty() || weatherCode.length() != 4 || weatherCode[0] != '1')
    return (UNKNOWN_FORECAST);
  return (decodeWeatherCode(weatherCode));
}

std::string extractTomorrowForecastCode(const std::string &apiResponse) {
  std::string codeStr = apiResponse;
  std::string searchString = "\"code\":";
  size_t pos = codeStr.find(searchString);
  if (pos == std::string::npos) return "";
  codeStr = codeStr.substr(pos + searchString.length());
  pos = codeStr.find(searchString);
  if (pos == std::string::npos) return "";
  codeStr = codeStr.substr(pos + searchString.length(), 4);
  Log::printLog(DEBUG_LOG, BOT_L, "Weather code: " + codeStr);
  return (codeStr);
}

eForecast decodeWeatherCode(const std::string &code) {
  if (code == "1000")
    return (SUNNY);
  else if (code == "1003" || code == "1006" || code == "1009")
    return (CLOUDY);
  else if (code == "1030" || code == "1135" || code == "1147")
    return (FOGGY);
  else if (code == "1087" || code == "1273" || code == "1276")
    return (THUNDER);
  else if (code == "1066" || code == "1069" || code == "1072")
    return (FROSTY);
  else if (isCodeSnowy(code))
    return (SNOWY);
  else if (code[1] == '0' || code[1] == '1' || code[1] == '2')
    return (RAINY);
  else
    return (UNKNOWN_FORECAST);
}

bool isCodeSnowy(const std::string &code) {
  const char *snowyCodesArray[] = SNOWY_CODES;
  stringVector snowyCodes(
      snowyCodesArray,
      snowyCodesArray + sizeof(snowyCodesArray) / sizeof(snowyCodesArray[0]));
  return (std::find(snowyCodes.begin(), snowyCodes.end(), code) !=
          snowyCodes.end());
}

/*============================================================================*/
/*       ASCII cat                                                            */
/*============================================================================*/
void Bot::sendAsciiCat(const std::string &nick, const stringVector &cat) {
  stringVector::const_iterator itEnd = cat.end();
  for (std::vector<std::string>::const_iterator it = cat.begin(); it != itEnd;
       ++it) {
    std::ostringstream oss;
    oss << "PRIVMSG " << nick << " :" << *it << "\r\n";
    sendMessageToServer(oss.str());
  }
}

void Bot::sendAsciiCatByCommand(BotRequest *request, eBotCommand command) {
  switch (command) {
    case HELLO:
      sendAsciiCat(request->clientNickname, _hello);
      break;
    case INSULTME:
      sendAsciiCat(request->clientNickname, _insultMeCat);
      break;
    case JOKE:
      sendAsciiCat(request->clientNickname, _jokeCat);
      break;
    case ADVICE:
      sendAsciiCat(request->clientNickname, _adviceCat);
      break;
    default:
      break;
  }
}

// void Bot::sendAsciiCatByCommand(BotRequest *request, eBotCommand command) {
//   if (command == HELLO) {
//     stringVector::const_iterator itEnd = _hello.end();
//     for (std::vector<std::string>::const_iterator it = _hello.begin();
//          it != itEnd; ++it) {
//       std::ostringstream oss;
//       oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
//       sendMessageToServer(oss.str());
//     }
//   }
//   else if (command == INSULTME) {
//     stringVector insults;
//     insults.push_back(INSULTME_CAT_1);
//     insults.push_back(INSULTME_CAT_2);
//     insults.push_back(INSULTME_CAT_3);
//     insults.push_back(INSULTME_CAT_4);

//     for (std::vector<std::string>::const_iterator it = insults.begin();
//          it != insults.end(); ++it) {
//       std::ostringstream oss;
//       oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
//       sendMessageToServer(oss.str());
//     }
//   } else if (command == JOKE) {
//     stringVector joke;
//     joke.push_back(JOKE_CAT_1);
//     joke.push_back(JOKE_CAT_2);
//     joke.push_back(JOKE_CAT_3);

//     for (std::vector<std::string>::const_iterator it = joke.begin();
//          it != joke.end(); ++it) {
//       std::ostringstream oss;
//       oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
//       sendMessageToServer(oss.str());
//     }
//   } else if (command == ADVICE) {
//     stringVector advice;
//     advice.push_back(ADVICE_CAT_1);
//     advice.push_back(ADVICE_CAT_2);
//     advice.push_back(ADVICE_CAT_3);
//     advice.push_back(ADVICE_CAT_4);

//     for (std::vector<std::string>::const_iterator it = advice.begin();
//          it != advice.end(); ++it) {
//       std::ostringstream oss;
//       oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
//       sendMessageToServer(oss.str());
//     }
//   } else {  // UNKNOWN / DEFAULT
//     stringVector unknown;
//     unknown.push_back(DEFAULT_CAT_1);
//     unknown.push_back(DEFAULT_CAT_2);
//     unknown.push_back(DEFAULT_CAT_3);
//     unknown.push_back(DEFAULT_CAT_4);

//     for (std::vector<std::string>::const_iterator it = unknown.begin();
//          it != unknown.end(); ++it) {
//       std::ostringstream oss;
//       oss << "PRIVMSG " << request->clientNickname << " :" << *it << "\r\n";
//       sendMessageToServer(oss.str());
//     }
//   }
// }

void Bot::sendAsciiCatForecast(const std::string &nick, eForecast forecast) {
  switch (forecast) {
    case SUNNY:
      sendAsciiCat(nick, _sunnyCat);
      break;
    case CLOUDY:
      sendAsciiCat(nick, _cloudyCat);
      break;
    case FOGGY:
      sendAsciiCat(nick, _foggyCat);
      break;
    case RAINY:
      sendAsciiCat(nick, _rainyCat);
      break;
    case SNOWY:
      sendAsciiCat(nick, _snowyCat);
      break;
    case THUNDER:
      sendAsciiCat(nick, _thunderCat);
      break;
    case FROSTY:
      sendAsciiCat(nick, _frostyCat);
      break;
    case UNKNOWN_FORECAST:
    default:
      sendAsciiCat(nick, _unknownWeatherCat);
      break;
  }
}
