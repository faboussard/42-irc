/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 13:00:32 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/26 13:11:29 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include "../includes/Log.hpp"

#include "../includes/colors.hpp"

void Log::printLog(eLogLevel level, eLogContext context,
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

std::string Log::logContext(eLogContext context) {
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
