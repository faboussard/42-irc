/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 08:40:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/12/05 10:43:13 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_PARSER_HPP_
#define INCLUDES_PARSER_HPP_

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "../includes/Config.hpp"
#include "../includes/Server.hpp"

const size_t MAX_MESSAGE_LENGTH = 512;

extern Config *gConfig;

typedef std::vector<std::pair<std::string, std::string> > stringPairsVector;

class Parser {
 private:
  Parser(void);

 public:
  static Command choseCommand(const std::string &command);
  static stringVector splitCommand(const std::string &command);
  static stringPairsVector parseCommandIntoPairs(const std::string &command,
                                                 const Client &client);
  static bool verifyPassword(std::string arg, std::string psd, Client *client);
  static bool verifyNick(const std::string &nick, Client *client,
                         clientsMap *cltMap);
  static bool verifyUser(const std::string &user, Client *client,
                         clientsMap *cltMap);
};

#endif  // INCLUDES_PARSER_HPP_
