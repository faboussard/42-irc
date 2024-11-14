/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:18:52 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/14 17:56:46 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "../../includes/Server.hpp"

// PRIVMSG <target>{,<target>} <:text to be sent>

//<target> is the nickname of a client or the name of a channel.

// If a message cannot be delivered to a channel, the server SHOULD respond
// with an ERR_CANNOTSENDTOCHAN (404) numeric to let the user know that this
// message could not be delivered.

// If <target> is a channel name, it may be prefixed with one or more channel
// membership prefix character (@ = STATUSMSG) and the message will be
// delivered only to operators.

// When the PRIVMSG message is sent from a server to a client and <target>
// starts with a dollar character ('$', 0x24), the message is a broadcast sent
// to all clients on one or multiple servers.

void Server::sendPrivmsgToClient(const Client &sender, const Client &receiver,
                                 const std::string &content) {
  std::string message = ":" + sender.getNickname() + " PRIVMSG " +
                        receiver.getNickname() + " " + content + "\r\n";
  receiver.receiveMessage(message);
}

void Server::broadcastToOperatorsOnly(const Client &sender,
                                      const Channel &channel,
                                      const std::string &command,
                                      const std::string &content) {
  std::string message = ":" + sender.getNickname() + " " + command + " " +
                        channel.getNameWithPrefix() + " :" + content + "\r\n";
  const clientPMap &operators = channel.getChannelOperators();
  clientPMap::const_iterator itEnd = operators.end();
  for (clientPMap::const_iterator it = operators.begin(); it != itEnd; ++it) {
    it->second->receiveMessage(message);
  }
}


void Server::broadcastToAllOperators(const Client &sender,
                                     const std::string &command,
                                     const std::string &content) {
  std::string message =
      ":" + sender.getNickname() + " " + command + " :" + content + "\r\n";

  for (channelsMap::const_iterator channelIt = _channels.begin();
       channelIt != _channels.end(); ++channelIt) {
    const clientPMap &operators = channelIt->second.getChannelOperators();
    for (clientPMap::const_iterator operatorIt = operators.begin();
         operatorIt != operators.end(); ++operatorIt) {
      operatorIt->second->receiveMessage(message);
    }
  }
}

// Fonction pour valider les arguments de privmsg
bool Server::isArgumentValid(const std::string &arg, const Client &client,
                             const std::vector<std::string> &targets,
                             const std::string &message) {
  size_t commaCount = std::count(arg.begin(), arg.end(), ',');
  if (commaCount > gConfig->getLimit(MAXTARGETS)) {
    send407TooManyTargets(client);
    return false;
  }

  // Analyser les arguments pour obtenir les cibles et le message
  if (message.empty() || message[0] != ':') {
    send412NoTextToSend(client);
    return false;
  }

  // verifier que chaque cible est homogene. pas de mix $ et @ par exemple

  size_t chanOpPrefixCount = 0;
  size_t channelsCount = 0;
  size_t clientsCount = 0;

  std::vector<std::string>::const_iterator itEnd = targets.end();
  for (std::vector<std::string>::const_iterator it = targets.begin();
       it != itEnd; ++it) {
    std::string target = *it;
    if (target[0] == CHAN_OP) chanOpPrefixCount++;
    if (target[0] == REG_CHAN) channelsCount++;
    if (target[0] == CHAN_OP && target[1] == REG_CHAN)
      target = target.substr(2);
    else if (target[0] == CHAN_OP || target[0] == REG_CHAN)
      target = target.substr(1);
    if (channelExists(target)) channelsCount++;
    if (findClientByNickname(target)) {
      clientsCount++;
    }
    if ((channelsCount || clientsCount || chanOpPrefixCount) !=
        commaCount + 1) {
      send407TooManyTargets(client);
      return false;
    }
  }
  return true;
}

bool Server::parseArguments(const std::string &arg, const Client &client,
                            std::vector<std::string> &targets,
                            std::string &message) {
  std::istringstream iss(arg);
  std::string target;
  std::getline(iss, target, ' ');
  if (target.empty()) {
    send411NoRecipient(client, "PRIVMSG");
    return (false);
  }
  while (!target.empty()) {
    std::getline(iss, target, ',');
    targets.push_back(target);
  }
  std::getline(iss, message, ' ');
  if (!isArgumentValid(arg, client, targets, message)) {
    return false;
  }
  return (true);
}

// Fonction principale privmsg
void Server::privmsg(int fd, const std::string &arg) {
  const Client &sender = _clients.at(fd);

  if (arg.empty()) {
    send461NeedMoreParams(sender, "PRIVMSG");
    return;
  }
  std::vector<std::string> targets;
  std::string message;
  if (parseArguments(arg, sender, targets, message) == false) {
    return;
  }

#ifdef DEBUG
  std::cout << "[arg]  " << arg << std::endl;
  std::cout << "[targets]  " << targets << std::endl;
  std::cout << "[message]  " << message << std::endl;
#endif

  // Valider les arguments

  bool isChannel = (targets[0][0] == REG_CHAN);
  bool isChanOpPrefix = (targets[0][0] == CHAN_OP);
  std::vector<std::string>::const_iterator itEnd = targets.end();
  for (std::vector<std::string>::const_iterator it = targets.begin();
       it != itEnd; ++it) {
    std::string target = *it;
    if (!channelExists(target) || !findClientByNickname(target)) {
      send401NoSuchNick(sender, target);
      return;
    }
    Channel &channel = _channels.at(target);
    if (channel.getMode().inviteOnly &&
        !channel.isClientInvited(sender.getFd())) {
      send404CannotSendToChan(sender, channel);
      return;
    }

    if (isChanOpPrefix) {
      broadcastToOperatorsOnly(sender, channel, "PRIVMSG", message);
    } else if (isChannel) {
      broadcastInChannel(sender, channel, "PRIVMSG", message);
    } else {
      sendPrivmsgToClient(sender, *findClientByNickname(target), message);
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
