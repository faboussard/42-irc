/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 08:40:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/28 14:56:20 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_PARSER_HPP_
#define INCLUDES_PARSER_HPP_

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../includes/Server.hpp"

typedef std::vector<std::pair<std::string, std::string> > commandVectorPairs;
class Parser {
 private:
  explicit Parser(void);

 public:
  static Command choseCommand(const std::string& command);
  static std::vector<std::string> splitCommand(const std::string& command);
  static commandVectorPairs parseCommandIntoPairs(std::string command);
  //  static std::string parseCommand(const std::vector<std::string> command);
  static void handleCommand(const std::string& command, int fd);
  //  static std::string returnMessage(const std::string& command);
  static bool verifyNick(std::string nick, clientsMap clientmap);
  static bool verifyUsername(const std::string& username, clientsMap clientmap);
};

#endif  // INCLUDES_PARSER_HPP_
