/* Copyright 2024 <yusengok>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 09:29:39 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/28 11:42:27 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_SERVERCONFIG_HPP_
#define INCLUDES_SERVERCONFIG_HPP_

#include <string>

#include "./utils.hpp"

/* Server information */
#define SRV_NAME "ircserv.localhost"
#define SRV_VERSION "1.0.0"
#define NETWORK_NAME "42IRC"

/* Modes */
#define USER_MODES "ior"       // List of available modes in server (Example)
#define CHANNEL_MODES "itkol"  // List of available modes in server (Example)

/* ISUPPORT Parameters */
#define PREFIX "(o)@"  // List of available client statuses in channels
#define TOKEN_PREFIX (std::string("PREFIX=") + PREFIX)

#define NICKLEN 9
#define TOKEN_NICKLEN (std::string("NICKLEN=") + toString(NICKLEN))

#define USERLEN 20
#define TOKEN_USERLEN (std::string("USERLEN=") + toString(USERLEN))

#define UNIQUE ""
#define TOKEN_UNIQUE (std::string("UNIQUE") + UNIQUE)

#define TOKENS \
  (TOKEN_PREFIX + std::string(" ") + TOKEN_NICKLEN + " " + TOKEN_USERLEN + \
   " " + TOKEN_UNIQUE)
// https://modern.ircdocs.horse/#rplisupport-parameters

#endif  // INCLUDES_SERVERCONFIG_HPP_
