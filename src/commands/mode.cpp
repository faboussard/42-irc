/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 10:02:17 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/21 22:24:48 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"
#include "../../includes/Parser.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/numericReplies.hpp"
#include "../../includes/utils.hpp"

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
//     case 't':  // Définir/supprimer les restrictions de la commande TOPIC
//     pour
//                // les opérateurs de canaux
//       if (plusMode)
//         _channels[channelName].activateTopicOpsOnlyMode();
//       else
//         _channels[channelName].deactivateTopicOpsOnlyMode();
//       break;
//     case 'k':  // Définir/supprimer la clé du canal (mot de passe)
//       if (plusMode) {
//         _channels[channelName].activateKeyMode(parsedArgument[2],
//         _clients[fd]); _channels[channelName].updateKey(parsedArgument[2]);
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

bool Server::isModeArgumentValid(const stringVector &modeStrings,
                                 const stringVector &arguments) {
  std::map<std::string, bool> modesRequiringArgument;
  modesRequiringArgument["+k"] = true;   // Mode +k nécessite un argument
  modesRequiringArgument["-k"] = false;  // Mode -k ne nécessite pas un argument
  modesRequiringArgument["+o"] = false;  // Mode +o ne nécessite pas un argument
  modesRequiringArgument["-o"] = false;  // Mode -o ne nécessite pas un argument
  modesRequiringArgument["+l"] = true;   // Mode +l nécessite un argument
  modesRequiringArgument["-l"] = false;  // Mode -l ne nécessite pas un argument
  modesRequiringArgument["+t"] = false;  // Mode +t ne nécessite pas un argument
  modesRequiringArgument["-t"] = false;  // Mode -t ne nécessite pas un argument
  modesRequiringArgument["+i"] = false;  // Mode +i ne nécessite pas un argument
  modesRequiringArgument["-i"] =
      false;  // Mode -i nne nécessite pas un argument
  size_t argumentIndex = 0;
  for (size_t i = 0; i < modeStrings.size(); ++i) {
    const std::string &modeString = modeStrings[i];
    for (size_t j = 1; j < modeString.size(); ++j) {
      std::string mode =
          modeString.substr(0, 1) +
          modeString[j];  // Extrait le signe et le caractère du mode
#ifdef DEBUG
      {
        std::ostringstream oss;
        oss << "Mode: " << mode << " | ArgumentIndex: " << argumentIndex
            << " | "
            << "Argument: " << arguments[argumentIndex] << " | "
            << "booleen:" << modesRequiringArgument[mode];
        printLog(DEBUG_LOG, COMMAND, oss.str());
      }
#endif
      if (modesRequiringArgument.find(mode) == modesRequiringArgument.end()) {
        return false;  // Mode inconnu
      }
      if (modesRequiringArgument[mode]) {
        if (argumentIndex >= arguments.size()) {
          return false;  // Argument manquant
        }
        ++argumentIndex;  // Consomme un argument
      }
    }
  }

  return true;
}

bool Server::isModeStringValid(const stringVector &argumentToCheck) {
  #ifdef DEBUG
  {
    std::ostringstream oss;
    oss << "argumentToCheck.size(): " << argumentToCheck.size();
    printLog(DEBUG_LOG, COMMAND, oss.str());
  }
  #endif
  if (argumentToCheck.size() < 1) {
    return false;
  }

  const std::string validModes = "itkol";
  const std::string plusMinus = "+-";
  for (size_t i = 0; i < argumentToCheck.size(); ++i) {
    const std::string &arg = argumentToCheck[i];

    if (arg[0] != '+' && arg[0] != '-') {
      #ifdef DEBUG
      {
        std::ostringstream oss;
        oss << "Mode invalide: " << arg;
        printLog(DEBUG_LOG, COMMAND, oss.str());
      }
      #endif
      return false;
    }

    for (size_t j = 1; j < arg.size(); ++j) {
      if (validModes.find(arg[j]) == std::string::npos) {
#ifdef DEBUG
        {
          std::ostringstream oss;
          oss << "Mode invalide: " << arg[j];
          printLog(DEBUG_LOG, COMMAND, oss.str());
        }
#endif
        return false;
      }
    }
  }
  return true;
}

bool Server::isChannelValid(int fd, const std::string &channel) {
  if (channel.empty() || channel[0] != REG_CHAN) {
    send461NeedMoreParams(_clients[fd], "MODE");
    return (false);
  }
  if (_channels.find(channel.substr(1)) == _channels.end()) {
    send403NoSuchChannel(_clients[fd], channel);
    return (false);
  }
  return (true);
}

void Server::mode(int fd, const std::string &arg) {
  const Client &client = _clients.at(fd);
  std::istringstream iss(arg);
  std::string channel, modeString, modeArguments;
  iss >> channel;
  if (isChannelValid(fd, channel) == false) return;
  if (!(iss >> modeString)) {
    send324Channelmodeis(_clients[fd], _channels[channel.substr(1)]);
    return;
  }
  KeyValuePairList modestringAndmodeArguments =
      parseCommandIntoKeyValuePairList(modeString, modeArguments);
  if (modestringAndmodeArguments.first.empty()) {
    std::cout << "hey" << std::endl;
    // send400UnknownError(
    //     client, modeString,
    //     "usage: mode format is <client> <channel> <modestring> <mode "
    //     "arguments> . it cannot contains commas or blankspaces");
    return;
  }
  stringVector modeStringVector = modestringAndmodeArguments.first;
  stringVector modeArgumentsVector = modestringAndmodeArguments.second;

  if (!isModeStringValid(modeStringVector) ||
      !isModeArgumentValid(modeStringVector, modeArgumentsVector)) {
    send472UnknownMode(client, modeArguments);
    return;
  }
  // ERR_CHANOPRIVSNEEDED (482) = f a user does not have appropriate privileges
  // to change modes on the target channel, the server MUST NOT process the
  // message, and
  // switchMode(fd, modeString, modeArguments);
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

TESTS:

 stringVector args1 = {"+i", "-t", "+k", "+o"};        // Valide
    stringVector args2 = {"+ik", "-tl"};                 // Valide
    stringVector args3 = {"i", "+x", "-y"};              // Invalide (manque +
ou caractères invalides) stringVector args4 = {"+i+t", "-l-k"};               //
Invalide (plusieurs signes) stringVector args5 = {"+", "-"}; // Invalide (trop
court) stringVector args6 = {"-ikt"};                       // Valide (tous
modes valides)
*/
