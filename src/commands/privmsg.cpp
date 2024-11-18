/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:18:52 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/18 12:05:09 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../includes/Server.hpp"

// PRIVMSG <target>{,<target>} <:text to be sent>
// <target> is the nickname of a client or the name of a channel.

void Server::sendPrivmsgToClient(const Client &sender, const Client &receiver,
                                 const std::string &content) {
  std::string message = ":" + sender.getNickname() + " PRIVMSG " +
                        receiver.getNickname() + " " + content + "\r\n";
  receiver.receiveMessage(message);
}

// Fonction pour valider les arguments de privmsg
bool Server::validPrivmsgTargets(const std::string &arg, const Client &client,
                          stringVector *targets) {
  size_t commaCount = std::count(arg.begin(), arg.end(), ',');
#ifdef DEBUG
{
  std::ostringstream oss;
  oss << "PRIVMSG: commaCount: " << commaCount;
  printLog(DEBUG_LOG, COMMAND, oss.str());
}
{
  std::ostringstream oss;
  oss << "PRIVMSG: MAXTARGETS: " << gConfig->getLimit(MAXTARGETS);
  printLog(DEBUG_LOG, COMMAND, oss.str());
}
  // std::cout << "commaCount: " << commaCount << std::endl;
  // std::cout << "MAXTARGETS: " << gConfig->getLimit(MAXTARGETS) << std::endl;
#endif
  if (commaCount + 1 > gConfig->getLimit(MAXTARGETS)) {
    send407TooManyTargets(client);
    return (false);
  }
  // verifier que chaque cible est homogene. pas de mix @ et # et user

  if (commaCount != 0) {
    size_t chanOpPrefixCount = 0;
    size_t channelsCount = 0;
    size_t clientsCount = 0;

    stringVector::iterator itEnd = (*targets).end();
    for (stringVector::iterator it = (*targets).begin(); it != itEnd; ++it) {
      std::string target = *it;
#ifdef DEBUG
      // std::cout << "target: " << target << std::endl;
      {
        std::ostringstream oss;
        oss << "PRIVMSG: target: " << target;
        printLog(DEBUG_LOG, COMMAND, oss.str());
      }
#endif
      if (target[0] == CHAN_OP)
        chanOpPrefixCount++;
      else if (target[0] == REG_CHAN)
        channelsCount++;
      else
        clientsCount++;
    }
    if (commaCount != 0 && channelsCount != commaCount + 1 &&
        clientsCount != commaCount + 1 && chanOpPrefixCount != commaCount + 1) {
      send407TooManyTargets(client);
      return (false);
    }
  }
  return (true);
}

bool Server::parsePrivmsgArguments(const std::string &arg, const Client &client,
                            std::vector<std::string> *targetsVector,
                            std::string *message) {
  // Lire le premier target
  if (arg.empty()) {
    send411NoRecipient(client, "PRIVMSG");
    return (false);
  }

  std::string::size_type spacePos = arg.find(" ");
  if (spacePos != std::string::npos) {
    *message = arg.substr(spacePos + 1);
  } else {
    *message = "";
  }
  if (message->empty() || (*message)[0] != ':') {
    send412NoTextToSend(client);
    return (false);
  }
  std::string targetsPart = arg.substr(0, spacePos);
  splitByCommaAndTrim(targetsPart, targetsVector);

#ifdef DEBUG
  // std::cout << "targetsPart: " << targetsPart << std::endl;
  // std::cout << "[in parsearg ]  " << message << std::endl;
  {
    std::ostringstream oss;
    oss << "PRIVMSG: targetsPart: " << targetsPart;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
  {
    std::ostringstream oss;
    oss << "PRIVMSG: " CYAN "in parsearg: " RESET << message;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
  // Vérifier si les arguments sont valides
  if (!validPrivmsgTargets(arg, client, targetsVector)) {
    return (false);
  }
  return (true);
}

// Fonction principale privmsg
void Server::privmsg(int fd, const std::string &arg) {
  const Client &sender = _clients.at(fd);

  stringVector targetsVector;
  std::string message;
  if (parsePrivmsgArguments(arg, sender, &targetsVector, &message) == false) {
    return;
  }
#ifdef DEBUG
  // std::cout << "[message]  " << message << std::endl;
  {
    std::ostringstream oss;
    oss << "PRIVMSG: " CYAN "message: " RESET << message;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif

  bool isChannel = (targetsVector[0][0] == REG_CHAN);
  bool isChanOpPrefix = (targetsVector[0][0] == CHAN_OP);
  stringVector::const_iterator itEnd = targetsVector.end();
  for (stringVector::const_iterator it = targetsVector.begin(); it != itEnd;
       ++it) {
    std::string target = *it;
    if (target[0] == CHAN_OP) target = target.substr(1);
#ifdef DEBUG
    // std::cout << "TRIMMED target: " << target << std::endl;
  {
    std::ostringstream oss;
    oss << "PRIVMSG: TRIMMED target: " << message;
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
#endif
    if (!channelExists(target) && !findClientByNickname(target)) {
      send401NoSuchNick(sender, target);
      return;
    }
    std::string messageWithoutColon = message.substr(1);

    if (channelExists(target)) {
      if (_channels.find(target.substr(1)) == _channels.end()) {
        std::cerr << "Error: Channel not found: " << target.substr(1)
                  << std::endl;
        return;
      }
      Channel &channel = _channels.at(target.substr(1));
      if (channel.getMode().inviteOnly &&
          !channel.isClientInvited(sender.getFd())) {
        send404CannotSendToChan(sender, channel);
        return;
      }
      if (isChanOpPrefix) {
        broadcastToOperatorsOnly(sender, channel, "PRIVMSG",
                                 messageWithoutColon);
      } else if (isChannel) {
        broadcastInChannel(sender, channel, "PRIVMSG", messageWithoutColon);
      }
    } else {
      Client *client = findClientByNickname(target);
      if (client) {
        sendPrivmsgToClient(sender, *client, messageWithoutColon);
      } else {
        std::cerr << "Error: Client not found for nickname: " << target
                  << std::endl;
      }
    }
  }
}

// Numeric Replies:
//*********************************************************************** */

//   ERR_NOSUCHNICK (401) - OK
// "<client> <nickname> :No such nick/channel"
// Indicates that no client can be found for the supplied nickname. The text
// used in the last param of this message may vary.
//*********************************************************************** */

// ERR_NOSUCHSERVER (402) = ne pas traite, pas demander par le sujet
//*********************************************************************** */

// ERR_CANNOTSENDTOCHAN (404) = OK

// ==> if sender is not invited to channel

// "<client> <channel> :Cannot send to channel"
// Indicates that the PRIVMSG / NOTICE could not be delivered to <channel>.
// The text used in the last param of this message may vary.

// This is generally sent in response to channel modes, such as a channel
// being moderated and the client not having permission to speak on the
// channel, or not being joined to a channel with the no external messages
// mode set.
//*********************************************************************** */
// ERR_TOOMANYTARGETS (407) = si melange $ et @ = OK
//*********************************************************************** */

// ERR_NORECIPIENT (411) = OK

// "<client> :No recipient given (<command>)"
// Returned by the PRIVMSG command to indicate the message wasn’t delivered
// because there was no recipient given.
//*********************************************************************** */

// ERR_NOTEXTTOSEND (412) = OK

// "<client> :No text to send"
// Returned by the PRIVMSG command to indicate the message wasn’t delivered
// because there was no text to send. ERR_NOTOPLEVEL (413) ERR_WILDTOPLEVEL
// (414) RPL_AWAY (301) = ne pas traiter, pas demande par le sujet

// Examples:

//   Command Examples:

//   PRIVMSG Angel :yes I'm receiving it !
//                                   ; Command to send a message to Angel.

//   PRIVMSG @#bunny :Hi! I have a problem!
//                                   ; Command to send a message to
//                                   and chanops on #bunny. This command is
//                                   functionally identical to the above
//                                   command.

//   PRIVMSG $#bunny :Hi! I have a problem! = NON GERE ! car un seul server
//                                   ; Command to send a message to halfops
//                                   and chanops on #bunny. This command is
//                                   functionally identical to the above
//                                   command.

// Message Examples:

//   :Angel PRIVMSG Wiz :Hello are you receiving this message ?
//                                   ; Message from Angel to Wiz.

//   :dan!~h@localhost PRIVMSG #coolpeople :Hi everyone!
//                                   ; Message from dan to the channel
//                                   #coolpeople
// If a message cannot be delivered to a channel, the server SHOULD respond
// with an ERR_CANNOTSENDTOCHAN (404) numeric to let the user know that this
// message could not be delivered.

// If <target> is a channel name, it may be prefixed with one or more channel
// membership prefix character (@ = STATUSMSG) and the message will be
// delivered only to operators.

// When the PRIVMSG message is sent from a server to a client and <target>
// starts with a dollar character ('$', 0x24), the message is a broadcast sent
// to all clients on one or multiple servers.
