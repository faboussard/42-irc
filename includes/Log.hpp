/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 12:57:53 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/03 20:08:22 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_LOG_HPP_
#define INCLUDES_LOG_HPP_

#include <string>

enum eLogLevel { DEBUG_LOG, INFO_LOG, NOTIFY_LOG, WARNING_LOG, ERROR_LOG };

enum eLogContext {
  SYSTEM,
  SIGNAL,
  PARSER,
  COMMAND,
  AUTH,
  CLIENT,
  CHANNEL,
  REPLY,
  BOT_L
};

/* log contexts */
#define SYSTEM_LOG "[System] "
#define SIGNAL_LOG "[Signal] "
#define PARSER_LOG "[Parser] "
#define COMMAND_LOG "[Command] "
#define AUTH_LOG "[Auth] "
#define CLIENT_LOG "[Client] "
#define CHANNEL_LOG "[Channel] "
#define REP_LOG "[Reply] "
#define BOT_LOG "[Bot] "


class Log {
 public:
  static void printLog(eLogLevel level, eLogContext context,
                       const std::string &message);

 private:
  Log(void);
  static std::string logContext(eLogContext context);
};

#endif  // INCLUDES_LOG_HPP_
