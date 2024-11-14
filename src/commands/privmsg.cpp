/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:18:52 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/14 12:24:19 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

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

void Server::broadcastToAllClients(const Client &sender,
                                   const std::string &command,
                                   const std::string &content) {
  std::string message =
      ":" + sender.getNickname() + " " + command + " :" + content + "\r\n";

  for (channelsMap::const_iterator channelIt = _channels.begin();
       channelIt != _channels.end(); ++channelIt) {
    const clientPMap &clients = channelIt->second.getChannelClients();
    for (clientPMap::const_iterator clientIt = clients.begin();
         clientIt != clients.end(); ++clientIt) {
      clientIt->second->receiveMessage(message);
    }
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

void Server::privmsg(int fd, const std::string &arg) {
  // PRIVMSG message
  //      Command: PRIVMSG

  //   Parameters: <target>{,<target>} <text to be sent>

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

  Client &client = _clients.at(fd);
  if (arg.empty()) {
    send461NeedMoreParams(client, "PRIVMSG");
    return;
  }
  std::istringstream iss(arg);
  std::string target, message;
  std::getline(iss, target, ' ');
  if (target.empty()) {
    send411NoRecipient(client, "PRIVMSG");
    return;
  }

  std::getline(iss, message, ' ');
  if (message.empty() || message[0] != ':') {
    send412NoTextToSend(client);
    return;
  }

  bool startsWithDollarPrefix = false;
  if (target[0] == '$') {
    target = target.substr(1);
    startsWithDollarPrefix = true;
  }

  bool startsWithOperatorPrefix = false;
  if (target[0] == STATUSMSG) {
    target = target.substr(1);
    startsWithOperatorPrefix = true;
  }

  // meme reponse dans le protocole irc si on ne trouve pas le client ou le
  // channel (alors que cest ambigu)
  bool isTargetAClient = false;
  Client *targetClient = findClientByNickname(target);
  if (targetClient == NULL) {
    send401NoSuchNick(client, target);
    return;
  } else
    isTargetAClient = true;

  bool isTargetAChannel = false;
  if (!channelExists(target.substr(1))) {
    send401NoSuchNick(client, target);
    return;
  } else
    isTargetAChannel = true;

  if (isTargetAClient == true) {
    sendPrivmsgToClient(client, *targetClient, message);
    return;
  }

  if (isTargetAChannel == true) {
    if (startsWithDollarPrefix == true &&
        startsWithOperatorPrefix == false) {  // gestion du $
      broadcastToAllClients(message);
      return;
    }

    if (startsWithDollarPrefix == true &&
        startsWithOperatorPrefix == true) {  // gestion du $
      broadcastToAllOperators(message);
      return;
    }

    if (startsWithOperatorPrefix == true) {  // gestion du @
      broadcastToOperatorsOnly(client, findChannelByName(target.substr(1)),
                               "PRIVMSG", message);
      return;
    }

    broadcastInChannel(client, findChannelByName(target.substr(1)), "PRIVMSG",
                       message);
  }
}

// Numeric Replies:

//   ERR_NOSUCHNICK (401) - OK
// "<client> <nickname> :No such nick/channel"
// Indicates that no client can be found for the supplied nickname. The text
// used in the last param of this message may vary.

// ERR_NOSUCHSERVER (402) = ne pas traite, pas demander par le sujet

// ERR_CANNOTSENDTOCHAN (404)

// "<client> <channel> :Cannot send to channel"
// Indicates that the PRIVMSG / NOTICE could not be delivered to <channel>.
// The text used in the last param of this message may vary.

// This is generally sent in response to channel modes, such as a channel
// being moderated and the client not having permission to speak on the
// channel, or not being joined to a channel with the no external messages
// mode set. ERR_TOOMANYTARGETS (407) = irc ne fournit pas de numeric reply
// specific. on va plutot considerer que lon doit specifier une seule target
// ERR_NORECIPIENT (411) = OK

// "<client> :No recipient given (<command>)"
// Returned by the PRIVMSG command to indicate the message wasn’t delivered
// because there was no recipient given.

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

//   PRIVMSG $#bunny :Hi! I have a problem!
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
