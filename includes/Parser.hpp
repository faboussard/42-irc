/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 08:40:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/25 15:37:52 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_PARSER_HPP_
#define INCLUDES_PARSER_HPP_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "../includes/Server.hpp"
// /JOIN quelquechose
// parser -> JOIN -> deuxieme partie

class Parser {
 private:
  explicit Parser(void);

 public:
  static Command choseCommand(const std::string& command);
  static std::vector<std::string> splitCommand(const std::string& command);
//  static std::string parseCommand(const std::vector<std::string> command);
  static void handleCommand(const std::string& command, int fd);
//  static std::string returnMessage(const std::string& command);
  static bool verifyNick(std::vector<std::string> &command, clientsMap clientmap);
};

#endif  // INCLUDES_PARSER_HPP_
