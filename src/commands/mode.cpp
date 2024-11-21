/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 10:02:17 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/21 23:06:08 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"
#include "../../includes/Parser.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/numericReplies.hpp"
#include "../../includes/utils.hpp"

// Parameters: <channel> {[+|-]|o|s|i|t|k|l} [<limit>] [<user>] [<ban mask>]

void Server::switchMode( Client *client, const std::string &channelName,
                        const stringVector &modeStrings,
                        const stringVector &argumentVector) {
  Channel *channel = &_channels[channelName.substr(1)];
  size_t argumentIndex = 0;

  for (size_t i = 0; i < modeStrings.size(); ++i) {
    const std::string &modeString = modeStrings[i];
    bool plusMode =
        (modeString[0] == '+');  // Déterminer si on ajoute ou enlève un mode

    for (size_t j = 1; j < modeString.size(); ++j) {
      const char &c = modeString[j];  // Caractère du mode actuel

      switch (c) {
        case 'i':  // Mode invitation uniquement
          if (plusMode)
            channel->activateInviteOnlyMode();
          else
            channel->deactivateInviteOnlyMode();
          break;

        case 't':  // Restrictions de modification de sujet
          if (plusMode)
            channel->activateTopicOpsOnlyMode();
          else
            channel->deactivateTopicOpsOnlyMode();
          break;

        case 'k':  // Gestion de clé (mot de passe)
          if (plusMode) {
            if (argumentIndex >= argumentVector.size()) {
              send461NeedMoreParams(*client, "MODE +k");
              return;  // Argument manquant
            }
            const std::string &key = argumentVector[argumentIndex++];
            channel->activateKeyMode(key, *client);
            channel->updateKey(key);
          } else {
            channel->deactivateKeyMode();
          }
          break;

        case 'o':  // Gestion des opérateurs
          if (plusMode) {
            if (!client->getNickname().empty() &&
                client->getNickname() == channel->getName()) {
              break;  // Ignorer si l'utilisateur essaie de s'auto-promouvoir
            }
            channel->addOperator(client);
          } else {
            channel->removeOperator(client);
          }
          break;

        case 'l':  // Limite d’utilisateurs
          if (plusMode) {
            if (argumentIndex >= argumentVector.size()) {
              send461NeedMoreParams(*client, "MODE +l");
              return;  // Argument manquant
            }
            int limit = std::atoi(argumentVector[argumentIndex++].c_str());
            channel->activateLimitMode(limit, *client);
          } else {
            channel->deactivateLimitMode();
          }
          break;

        default:
          send472UnknownMode(*client, std::string(1, c));
          return;  // Mode invalide
      }
    }
  }
}

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
#ifdef DEBUG
    {
      std::ostringstream oss;
      oss << "arg: " << arg;
      printLog(DEBUG_LOG, COMMAND, oss.str());
    }
#endif

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
  const Channel &channelObj = _channels[channel.substr(1)];
  if (!(iss >> modeString)) {
    send324Channelmodeis(client, channelObj);
    return;
  }
  if (channelObj.isOperator(fd) == false) {
    send482ChanOPrivsNeeded(client, channelObj);
    return;
  }
  KeyValuePairList modestringAndmodeArguments =
      parseCommandIntoKeyValuePairList(modeString, modeArguments);
  if (modestringAndmodeArguments.first.empty()) {
    // send400UnknownError(
    //     client, modeString,
    //     "usage: mode format is <client> <channel> <modestring> <mode "
    //     "arguments> . it cannot contains commas or blankspaces");
    return;
  }
  stringVector modeStringVector = modestringAndmodeArguments.first;
  stringVector modeArgumentsVector = modestringAndmodeArguments.second;

  if (!isModeStringValid(modeStringVector) ||
      (modeArgumentsVector.size() > 0 &&
       !isModeArgumentValid(modeStringVector, modeArgumentsVector))) {
    send472UnknownMode(client, modeArguments);
    return;
  }
  switchMode(&_clients[fd], channel, modeStringVector, modeArgumentsVector);
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
