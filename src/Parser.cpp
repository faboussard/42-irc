/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 09:46:04 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/29 10:07:30 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

#include "../includes/colors.hpp"

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

bool Parser::verifyPassword(std::string arg, std::string psd, Client& client) {
  if (arg.empty()) {
    // sendNumericReply(464, "ERR_PASSWDMISMATCH");
    return (false);
  }
  if (arg != psd) {
    // sendNumericReply(464, "ERR_PASSWDMISMATCH");
    return (false);
  }
  client.declarePasswordGiven();
  return (true);
}
//The password supplied must match the one defined in the server configuration.
// WARNING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// It is possible to send multiple PASS commands before registering but
// only the last one sent is used for verification and it may not be changed 
// once the client has been registered.
// The PASS command is used to set a ‘connection password’. If set, the password must be set before any attempt to register the connection is made. This requires that clients send a PASS command before sending the NICK / USER combination.

// The password supplied must match the one defined in the server configuration. It is possible to send multiple PASS commands before registering but only the last one sent is used for verification and it may not be changed once the client has been registered.

// If the password supplied does not match the password expected by the server, then the server SHOULD send ERR_PASSWDMISMATCH (464) and MAY then close the connection with ERROR. Servers MUST send at least one of these two messages.

// Servers may also consider requiring SASL authentication upon connection as an alternative to this, when more information or an alternate form of identity verification is desired.

// Numeric replies:

//     ERR_NEEDMOREPARAMS (461)
//     ERR_ALREADYREGISTERED (462)
//     ERR_PASSWDMISMATCH (464)

bool Parser::verifyNick(std::string nick, Client& client, clientsMap cltMap) {
  if (nick.empty()) {
    // sendNumericReply(431, "ERR_NONICKNAMEGIVEN");
    return (false);
  }
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
  clientsMap::iterator itEnd = cltMap.end();
  for (clientsMap::iterator it = cltMap.begin(); it != itEnd; ++it) {
    if (it->second.getNickName() == nick) {
      // sendNumericReply(433, "ERR_NICKNAMEINUSE");
      return (false);
    }
  }
  client.setNickname(nick);
  return (true);
}

std::vector<std::string> fillVectorString(const std::string str) {
  std::vector<std::string> result;
  std::istringstream iss(str);
  std::string token;

  while (iss >> token) {
    result.push_back(token);
  }
  return (result);
}

bool Parser::verifyUser(std::string user, Client& client, clientsMap cltMap) {
  if (user.empty()) {
    // ERR_NEEDMOREPARAMS (461)
    return (false);
  }
  // std::istringstream iss(user);
  // std::vector<std::string> fields;
  // std::string field;
  // while (iss >> field) {
  //   fields.push_back(field);
  // }
  std::vector<std::string> fields = fillVectorString(user);
  if (fields.size() != 4) {
    std::cout << "fields.size() != 4" << std::endl;
    // ERR_NEEDMOREPARAMS (461)
    return (false);
  }
  size_t size = fields[0].size();
  for (size_t i = 0; i < size; ++i) {
    if (std::isspace(fields[0].at(i)) || std::iscntrl(fields[0].at(i))) {
      std::cout <<  "space or control char" << std::endl;
      return (false);
    }
  }
  clientsMap::iterator itEnd = cltMap.end();
  for (clientsMap::iterator it = cltMap.begin(); it != itEnd; ++it) {
    if (it->second.getUserName() == user) {
      // sendNumericReply(462, "ERR_ALREADYREGISTERED");
      std::cout << "username already registered" << std::endl;
      return (false);
    }
  }
  client.setUserName(user);
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
// Selon la spécification du protocole IRC, une fois qu'un client est enregistré (c'est-à-dire après avoir envoyé les commandes NICK et USER avec succès), il ne peut plus changer son mot de passe en envoyant une nouvelle commande PASS. Toute tentative de changer le mot de passe après l'enregistrement doit être ignorée par le serveur.

// Comportement Attendu
// Avant l'Enregistrement : Le client peut envoyer plusieurs commandes PASS, mais seule la dernière est utilisée pour la vérification.
// Après l'Enregistrement : Toute tentative d'envoyer une commande PASS doit être ignorée, et le serveur ne doit pas fermer la connexion du client.
// Mise à Jour de la Méthode handleInitialMessage
// Vous pouvez mettre à jour votre méthode handleInitialMessage pour gérer cette situation.


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
