/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 08:40:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/29 10:40:20 by mbernard         ###   ########.fr       */
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
  static bool verifyPassword(std::string arg, std::string psd, Client& client);
  static bool verifyNick(std::string nick, Client& client, clientsMap cltMap);
  static bool verifyUser(std::string user, Client& client, clientsMap cltMap);
};

#endif  // INCLUDES_PARSER_HPP_