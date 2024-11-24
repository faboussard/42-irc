/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanny <faboussa@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 10:02:17 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/24 12:13:28 by fanny            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"
#include "../../includes/Parser.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/numericReplies.hpp"
#include "../../includes/utils.hpp"

// Parameters: <channel> {[+|-]|o|s|i|t|k|l} [<limit>] [<user>] [<ban mask>]

void Server::switchMode(Client *client, const std::string &channelName,
                        const stringVector &modeStrings,
                        const stringVector &argumentVector) {
  Channel *channel = &_channels[channelName.substr(1)];
  size_t argumentIndex = 0;

  for (size_t i = 0; i < modeStrings.size(); ++i) {
    const std::string &modeString = modeStrings[i];
    bool plusMode = (modeString[0] == '+');

    for (size_t j = 1; j < modeString.size(); ++j) {
      const char &c = modeString[j];

      if (c == 'i' || c == 't') {
        if (plusMode)
          (c == 'i') ? channel->activateInviteOnlyMode()
                     : channel->activateTopicOpsOnlyMode();
        else
          (c == 'i') ? channel->deactivateInviteOnlyMode()
                     : channel->deactivateTopicOpsOnlyMode();
      } else if (c == 'k') {
        if (plusMode) {
          if (argumentIndex >= argumentVector.size()) {
            send461NeedMoreParams(*client, "MODE +k");
            return;
          }
          const std::string &key = argumentVector[argumentIndex++];
          channel->activateKeyMode(key, *client);
          channel->updateKey(key);
        } else {
          channel->deactivateKeyMode();
        }
      } else if (c == 'o') {
        if (argumentIndex >= argumentVector.size()) {
          send461NeedMoreParams(*client, "MODE +o/-o");
          return;
        }
        Client *clientToOp =
            findClientByNickname(argumentVector[argumentIndex]);
        if (clientToOp == NULL) {
          send401NoSuchNick(*client, argumentVector[argumentIndex]);
          return;
        }
        if (plusMode) {
          if (!client->getNickname().empty() &&
              client->getNickname() == argumentVector[argumentIndex++]) {
            break;
          }
          channel->addOperator(clientToOp);
        } else {
          channel->removeOperator(clientToOp);
        }
      } else if (c == 'l') {
        if (plusMode) {
          if (argumentIndex >= argumentVector.size()) {
            send461NeedMoreParams(*client, "MODE +l");
            return;
          }
          const std::string &limitStr = argumentVector[argumentIndex++];
          if (!isNumeric(limitStr)) {
            sendNotice(*client, "MODE +l argument must be a numeric value.");
            return;
          }
          int limit = std::atoi(limitStr.c_str());
          channel->activateLimitMode(limit, *client);
        } else {
          channel->deactivateLimitMode();
        }
      }
    }
  }
}

std::string Server::checkArguments(const stringVector &modeStrings,
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
  modesRequiringArgument["-i"] = false;  // Mode -i ne nécessite pas un argument
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
        oss << "Mode: " << mode << " | "
            << "Argument: " << (argumentIndex < arguments.size() ? arguments[argumentIndex] : "N/A") << " | "
            << "booleen:" << modesRequiringArgument[mode];
        printLog(DEBUG_LOG, COMMAND, oss.str());
      }
#endif
      if (modesRequiringArgument[mode]) {
        if (argumentIndex >= arguments.size()) {
          std::string message = "missing argument " + mode;
          return message;
        }
        ++argumentIndex;
      }
    }
  }

  return "";
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

  if (channelObj.isOperator(fd) == false) {
    send482ChanOPrivsNeeded(client, channelObj);
    return;
  }
  std::string remainingArgs;
  std::getline(iss, remainingArgs);
  KeyValuePairList modestringAndArguments = parseMode(remainingArgs);
  if (modestringAndArguments.first.empty()) {
    send324Channelmodeis(client, channelObj);
    return;
  }
  stringVector modestringVector = modestringAndArguments.first;
  stringVector modeArgumentsVector = modestringAndArguments.second;

  if (!isModeStringValid(modestringVector)) {
    send472UnknownMode(client, modeArguments);
    sendNotice(client, "usage: MODE <channel> {[+|-]|o|i|t|k|l} [<arguments>]");
    return;
  }
  std::string error = checkArguments(modestringVector, modeArgumentsVector);
  if (!error.empty()) {
    send461NeedMoreParams(client, error);
    return;
  }
  switchMode(&_clients[fd], channel, modestringVector, modeArgumentsVector);
}

stringVector split(const std::string &str) {
  stringVector tokens;
  std::istringstream iss(str);
  std::string token;
  while (iss >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

KeyValuePairList Server::parseMode(const std::string &arg) {
  stringVector keyVector;
  stringVector valueVector;
  KeyValuePairList list;

  // Utilisation de la fonction split pour découper la chaîne
  stringVector tokens = split(arg);

  // Parcours des tokens pour les classer dans keyVector ou valueVector
  for (size_t i = 0; i < tokens.size(); ++i) {
    if (!tokens[i].empty() && (tokens[i][0] == '+' || tokens[i][0] == '-')) {
      keyVector.push_back(tokens[i]);
    } else {
      valueVector.push_back(tokens[i]);
    }
  }

#ifdef DEBUG
  {
    std::ostringstream before, afterKey, afterValue;
    before << "arg: Before split and process: " << arg;
    afterKey << "keyVector: ";
    for (size_t i = 0; i < keyVector.size(); ++i)
      afterKey << keyVector[i] << "|";
    afterValue << "valueVector: ";
    for (size_t i = 0; i < valueVector.size(); ++i)
      afterValue << valueVector[i] << "|";
    Server::printLog(DEBUG_LOG, COMMAND, before.str());
    Server::printLog(DEBUG_LOG, COMMAND, afterKey.str());
    Server::printLog(DEBUG_LOG, COMMAND, afterValue.str());
  }
#endif

  // Remplir la liste avec les clés et les valeurs
  list.first = keyVector;
  list.second = valueVector;
  return list;
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
ou caractères invalides)
stringVector args4 = {"+i+t", "-l-k"};               //Invalide (plusieurs
signes) stringVector args5 = {"+", "-"}; // Invalide (trop court) stringVector
args6 = {"-ikt"};   // Valide (tousmodes valides)
*/
