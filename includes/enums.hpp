/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enums.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 13:59:17 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/28 16:57:56 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_ENUMS_HPP_
#define INCLUDES_ENUMS_HPP_

enum Command {
  JOIN,
  KICK,
  INVITE,
  TOPIC,
  MODE,
  LIST,
  NICK,
  PRIVMSG,
  QUIT,
  PING,
  PONG,
  CAP,
  USER,
  PASS,
  WHO,
  UNKNOWN
};

enum eBotCommand {
  MENU,
  JOKE,
  INSULTME,
  ADVICE,
  WEATHER,
  RANDOM_BOT_COMMAND,
  UNKNOWN_BOT_COMMAND
};

#endif  // INCLUDES_ENUMS_HPP_
