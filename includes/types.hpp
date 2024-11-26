/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 13:59:17 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/21 14:05:55 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_TYPES_HPP_
#define INCLUDES_TYPES_HPP_

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
  CAP,
  USER,
  PASS,
  WHO,
  BOT,
  WEATHER,
  TRANSLATE,
  ASCII_ART,
  UNKNOWN
};

#endif  // INCLUDES_TYPES_HPP_
