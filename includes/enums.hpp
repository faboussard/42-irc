/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enums.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 13:59:17 by yusengok          #+#    #+#             */
/*   Updated: 2024/12/02 15:17:03 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_ENUMS_HPP_
#define INCLUDES_ENUMS_HPP_

enum Command {
  JOIN,
  PART,
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
  UNKNOWN_BOT_COMMAND
};

#endif  // INCLUDES_ENUMS_HPP_
