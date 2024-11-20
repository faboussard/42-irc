/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 10:02:17 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/20 14:00:59 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"
#include "../../includes/Parser.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/numericReplies.hpp"

// Parameters: <channel> {[+|-]|o|s|i|t|k|l} [<limit>] [<user>] [<ban mask>]

// void Server::switchMode(int fd, const char &c, const bool &plusMode,
//                         const stringVector &parsedArgument) {
//   u_int8_t i = 2;
//   switch (c) {
//     case 'i':  // Définir/supprimer le canal sur invitation uniquement
//       if (plusMode)
//         _channels[channelName].activateInviteOnlyMode();
//       else
//         _channels[channelName].deactivateInviteOnlyMode();
//       break;
//     case 't':  // Définir/supprimer les restrictions de la commande TOPIC pour
//                // les opérateurs de canaux
//       if (plusMode)
//         _channels[channelName].activateTopicOpsOnlyMode();
//       else
//         _channels[channelName].deactivateTopicOpsOnlyMode();
//       break;
//     case 'k':  // Définir/supprimer la clé du canal (mot de passe)
//       if (plusMode) {
//         _channels[channelName].activateKeyMode(parsedArgument[2], _clients[fd]);
//         _channels[channelName].updateKey(parsedArgument[2]);
//       } else {
//         _channels[channelName].deactivateKeyMode();
//       }
//       break;
//     case 'o':  // Donner/retirer le privilège de l’opérateur de canal
//                //  (pas plus de 3 à la fois)
//       //  If a user attempts to make themselves an operator using the "+o"
//       //  flag, the attempt should be ignored.
//       if (plusMode) {
//         _channels[channelName].addOperator(&_clients[fd]);
//       } else {
//         _channels[channelName].removeOperator(&_clients[fd]);
//       }
//       break;
//     case 'l':  // Définir/supprimer la limite d’utilisateurs pour le canal
//       if (plusMode) {
//         int limit = std::atoi(parsedArgument[2].c_str());
//         _channels[channelName].activateLimitMode(limit, _clients[fd]);
//       } else {
//         _channels[channelName].deactivateLimitMode();
//       }
//       break;
//     default:
//       //      send696InvalidModeParam(_clients[fd], c);
//       break;
//   }
// }

//  bool Server::modeArgsAreCorrect(const stringVector parsedArgument) {}
// static size_t countNbParamsMustBe(const std::string &str,
//                                   size_t *nbParamsMustBe, size_t *nbO) {
//   size_t i = 0;
//   bool excessivePlusOrMinus = false;
//   while (str[i]) {
//     if (str[i] == 'o' || str[i] == 'k' || str[i] == 'l') ++(*nbParamsMustBe);
//     if (str[i] == 'o') ++(*nbO);
//     if (str[i] == '+' || str[i] == '-') {
//       excessivePlusOrMinus = true;
//       break;
//     }
//     ++i;
//   }
//   if (excessivePlusOrMinus || *nbO > 3) *nbParamsMustBe = SIZE_MAX;
// }

stringVectorPairs parseModeIntoPairs(const std::string &args) {
  std::string channel, token;
  stringVector modeStringVector;
  stringVector modeArguments;

  std::istringstream iss(args);
  stringVectorPairs modeStringAndArguments;

  iss >> channel;
  while (iss >> token) {

  return (result);
}

bool Server::modeChannelNameIsCorrect(int fd, const std::string &arg) {
  std::istringstream iss(arg);
  std::string channel, modestring;

  if (!(iss >> channel)) {
    send461NeedMoreParams(_clients[fd], "MODE");
    return (false);
  }
  if (_channels.find(channel.substr(1)) == _channels.end()) {
    send403NoSuchChannel(_clients[fd], channel);
    return (false);
  }
  if (!(iss >> modestring)) {
    send324Channelmodeis(_clients[fd], _channels[channel.substr(1)]);
    return (false);
  }
  return (true);
}

void Server::mode(int fd, const std::string &arg) {
  if (modeChannelNameIsCorrect(fd, arg) == false) return;
  pairOfStringVectors pairedArgument = parseModeIntoPairs(arg);
  // size_t i = 0;
  // bool plusMode = true;
  // if (parsedArgument[1][0] == '+' || parsedArgument[1][0] == '-') {
  //   ++i;
  //   if (parsedArgument[0][0] == '-') plusMode = false;
  // } else {
  //   // WRONG PARAM
  //   return;
  // }
  // while (parsedArgument[1][i]) {
  //   switchMode(fd, parsedArgument[1][i], plusMode, parsedArgument);
  //   ++i;
  // }
}

// ======== MODE <prefix><channel>
// ============================================= Replies:
// -- 324 RPL_CHANNELMODEIS:
// :<server> 324 <client> <channel> <modestring> <mode arguments>...
// e.g. :irc.example.com 324 Alice #chatroom +itkl secretpass 25

// ======== MODE <prefix><channel> <modestring> <target nick>
// ================== e.g. MODE #channel +o <target nick> Replies:
// -- 441 ERR_USERNOTINCHANNEL:
// :<server> 441 <client> <target nick> <channel> :They aren't on that channel
// The target isn’t joined to the channel

/*
  MODE - Changer le mode du channel :
  — i : Définir/supprimer le canal sur invitation uniquement
  — t : Définir/supprimer les restrictions de la commande TOPIC pour les
opérateurs de canaux — k : Définir/supprimer la clé du canal (mot de passe) —
o : Donner/retirer le privilège de l’opérateur de canal (pas plus de 3 à la
fois) — l : Définir/supprimer la limite d’utilisateurs pour le canal 4.2.3
Mode message

      Command: MODE

   The MODE command is a dual-purpose command in IRC.  It allows both
   usernames and channels to have their mode changed.  The rationale for
   this choice is that one day nicknames will be obsolete and the
   equivalent property will be the channel.

   When parsing MODE messages, it is recommended that the entire message
   be parsed first and then the changes which resulted then passed on.

4.2.3.1 Channel modes

   Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]
               [<ban mask>]

   The MODE command is provided so that channel operators may change the
   characteristics of `their' channel.  It is also required that servers
   be able to change channel modes so that channel operators may be
   created.

   The various modes available for channels are as follows:

           i - invite-only channel flag;
           t - topic settable by channel operator only flag;
           o - give/take channel operator privileges;
           k - set a channel key (password).
           l - set the user limit to channel;


*/
