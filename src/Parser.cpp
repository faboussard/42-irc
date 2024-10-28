/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/28 15:49:40 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

#include "../includes/colors.hpp"
#include "Parser.hpp"

Command Parser::choseCommand(const std::string& command) {
  if (command == "JOIN") {
    return (JOIN);
  } else if (command == "KICK") {
    return (KICK);
  } else if (command == "INVITE") {
    return (INVITE);
  } else if (command == "TOPIC") {
    return (TOPIC);
  } else if (command == "MODE") {
    return (MODE);
  } else if (command == "LIST") {
    return (LIST);
  } else if (command == "NOTICE") {
    return (NOTICE);
  } else if (command == "NICK") {
    return (NICK);
  } else if (command == "PRIVMSG") {
    return (PRIVMSG);
  } else if (command == "QUIT") {
    return (QUIT);
  } else if (command == "PING") {
    return (PING);
  }
  return (UNKNOWN);
}

bool Parser::verifyNick(std::string nick, clientsMap clientmap) {
  if (nick.empty()) {
    // sendNumericReply(431, "ERR_NONICKNAMEGIVEN");
    return (false);
  }
  std::cout << "NICK IS " << nick << std::endl;
  size_t size = nick.size();
  if (size > 9 || std::isdigit(nick[0])) {
    // sendNumericReply(432, "ERR_ERRONEUSNICKNAME");
    return (false);
  }
  for (size_t i = 0; i < size; ++i) {
    if (!std::isalnum(nick[i]) && nick[i] != '[' && nick[i] != ']' &&
        nick[i] != '{' && nick[i] != '}' && nick[i] != '\\' && nick[i] != '|' &&
        nick[i] != '`' && nick[i] != '_' && nick[i] != '^' && nick[i] != '-') {
      // sendNumericReply(432, "ERR_ERRONEUSNICKNAME");
      return (false);
    }
  }
  clientsMap::iterator itEnd = clientmap.end();
  for (clientsMap::iterator it = clientmap.begin(); it != itEnd; ++it) {
    if (it->second.getNickName() == nick) {
      // sendNumericReply(433, "ERR_NICKNAMEINUSE");
      return (false);
    }
  }
  return (true);
}

bool Parser::verifyUsername(const std::string& username, clientsMap clientmap) {
  if (username.empty()) {
    // ERR_NEEDMOREPARAMS (461)
    return (false);
  }

  std::istringstream iss(username);
  std::vector<std::string> fields;
  std::string field;
  while (iss >> field) {
    fields.push_back(field);
  }
  if (fields.size() != 4) {
    std::cout << "fields.size() != 4" << std::endl;
      // ERR_NEEDMOREPARAMS (461)
    return (false);
  }

  size_t size = fields[0].size();
  for (size_t i = 0; i < size; ++i) {
    if (std::isspace(fields[0].at(i)) || std::iscntrl(fields[0].at(i))) {
      std::cout << "space or control char" << std::endl;
      return (false);
    }
  }
  clientsMap::iterator itEnd = clientmap.end();
  for (clientsMap::iterator it = clientmap.begin(); it != itEnd; ++it) {
    if (it->second.getUserName() == username) {
      // sendNumericReply(462, "ERR_ALREADYREGISTERED");
      std::cout << "username already registered" << std::endl;
      return (false);
    }
  }
  return (true);
}

// The NICK command is used to give the client a nickname or change the previous
// one.

// If the server receives a NICK command from a client where the desired
// nickname is already in use on the network, it should issue an
// ERR_NICKNAMEINUSE numeric and ignore the NICK command. If the server does not
// accept the new nickname supplied by the client as valid (for instance, due to
// containing invalid characters), it should issue an ERR_ERRONEUSNICKNAME
// numeric and ignore the NICK command.
// Servers MUST allow at least
// all alphanumerical characters,
// square and curly brackets ([]{}),
// backslashes (\),
// and pipe (|) characters in nicknames,
// and MAY disallow digits as the first character.
// Servers MAY allow extra characters, as long as they do not
// introduce ambiguity in other commands, including:
//     no leading # character or other character advertized in CHANTYPES
//     no leading colon (:)
//     no ASCII space
// If the server does not receive the <nickname> parameter with the NICK
// command, it should issue an ERR_NONICKNAMEGIVEN numeric and ignore the NICK
// command.

// The NICK message may be sent from the server to clients to acknowledge their
// NICK command was successful, and to inform other clients about the change of
// nickname. In these cases, the <source> of the message will be the old
// nickname [ [ "!" user ] "@" host ] of the user who is changing their
// nickname.

// Numeric Replies:
//     ERR_NONICKNAMEGIVEN (431)
//     ERR_ERRONEUSNICKNAME (432)
//     ERR_NICKNAMEINUSE (433)
//     ERR_NICKCOLLISION (436)

std::vector<std::string> Parser::splitCommand(const std::string& command) {
  std::istringstream iss(command);
  std::string token;
  std::vector<std::string> message;

  while (iss >> token) {
    message.push_back(token);
    token.clear();
  }
  return (message);  // /join #channel /nick nickname
}

std::vector<std::string> split(const std::string& str, char delim) {
  std::vector<std::string> result;
  std::stringstream ss(str);
  std::string item;

  while (getline(ss, item, delim)) {
    if (item.empty()) continue;
    result.push_back(item);
  }

  return (result);
}

commandVectorPairs Parser::parseCommandIntoPairs(std::string command) {
  std::vector<std::string> cmds = split(command, '\n');
  commandVectorPairs result;
  std::string token;
  std::pair<std::string, std::string> pair;
  size_t size = cmds.size();

  for (size_t i = 0; i < size; ++i) {
    cmds[i].erase(cmds[i].find_last_not_of(" \n\r\t") + 1);
    std::string firstPart = cmds[i].substr(0, cmds[i].find_first_of(" "));
    std::string secondPart = cmds[i].substr(cmds[i].find_first_of(" ") + 1);
    pair = std::make_pair(firstPart, secondPart);
    std::cout << CYAN "pair.first : " << pair.first << std::endl;
    std::cout << BLUE "pair.second : " << pair.second << RESET << std::endl;
    result.push_back(pair);
  }

  return (result);
}

void Parser::handleCommand(const std::string& command, int fd) {
  static_cast<void>(fd);
  if (command.empty()) {
    return;
  } else if (command == "JOIN") {
    // Ajouter le client au canal
  } else if (command == "KICK") {
    // Exclure un client du canal
  } else if (command == "INVITE") {
    // Notice
  } else if (command == "TOPIC") {
    // Changer le sujet du canal
  } else if (command == "MODE") {
    // Changer le sujet du canal
  } else if (command == "LIST") {
    // Lister les canaux
  } else if (command == "NOTICE") {
    // Notice}
  } else if (command == "NICK") {
    // change nickname
  } else if (command == "PRIVMSG") {
    // Envoyer un message privé
  } else if (command == "QUIT") {
    // Déconnecter le client
  } else if (command == "PING") {
    // client.sendNumericReply(1, "PONG");
  } else {
    // Commande inconnue
  }
}
