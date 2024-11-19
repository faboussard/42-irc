/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 10:02:17 by yusengok          #+#    #+#             */
/*   Updated: 2024/11/19 14:27:02 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"
#include "../../includes/Parser.hpp"
#include "../../includes/Server.hpp"

// Parameters: <channel> {[+|-]|o|s|i|t|k|l} [<limit>] [<user>] [<ban mask>]

void Server::switchMode(int fd, const char &c, const bool &plusMode,
                        const stringVector &parsedArgument) {
  std::string channelName = parsedArgument[0];
  u_int8_t i = 2;
  switch (c) {
    case 'i':  // Définir/supprimer le canal sur invitation uniquement
      if (plusMode)
        _channels[channelName].activateInviteOnlyMode();
      else
        _channels[channelName].deactivateInviteOnlyMode();
      break;
    case 't':  // Définir/supprimer les restrictions de la commande TOPIC pour
               // les opérateurs de canaux
      if (plusMode)
        _channels[channelName].activateTopicOpsOnlyMode();
      else
        _channels[channelName].deactivateTopicOpsOnlyMode();
      break;
    case 'k':  // Définir/supprimer la clé du canal (mot de passe)
      if (plusMode) {
        _channels[channelName].activateKeyMode(parsedArgument[2], _clients[fd]);
        _channels[channelName].updateKey(parsedArgument[2]);
      } else {
        _channels[channelName].deactivateKeyMode();
      }
      break;
    case 'o':  // Donner/retirer le privilège de l’opérateur de canal
               //  (pas plus de 3 à la fois)
      //  If a user attempts to make themselves an operator using the "+o"
      //  flag, the attempt should be ignored.
      if (plusMode) {
        _channels[channelName].addOperator(&_clients[fd]);
      } else {
        _channels[channelName].removeOperator(&_clients[fd]);
      }
      break;
    case 'l':  // Définir/supprimer la limite d’utilisateurs pour le canal
      if (plusMode) {
        int limit = std::atoi(parsedArgument[2].c_str());
        _channels[channelName].activateLimitMode(limit, _clients[fd]);
      } else {
        _channels[channelName].deactivateLimitMode();
      }
      break;
    default:
      //      send696InvalidModeParam(_clients[fd], c);
      break;
  }
}

//  bool Server::modeArgsAreCorrect(const stringVector parsedArgument) {}

bool Server::modeChannelNameIsCorrect(int fd, const std::string &arg) {
  std::istringstream iss(arg);
  std::string channel, arg1;

  if (!(iss >> channel) || !(iss >> arg1)) {
    send461NeedMoreParams(_clients[fd], "MODE");
    return (false);
  }
  if (_channels.find(channel) == _channels.end()) {
    send403NoSuchChannel(_clients[fd], channel);
    return (false);
  }
  return (true);
}

void Server::mode(int fd, const std::string &arg) {
  if (modeChannelNameIsCorrect(fd, arg) == false) return;
  stringVector parsedArgument = Parser::splitCommand(arg);
  commandVectorPairs pairedArgument = Parser::parseModeIntoPairs(arg);
  size_t i = 0;
  bool plusMode = true;
  if (parsedArgument[1][0] == '+' || parsedArgument[1][0] == '-') {
    ++i;
    if (parsedArgument[0][0] == '-') plusMode = false;
  } else {
    // WRONG PARAM
    return;
  }
  while (parsedArgument[1][i]) {
    switchMode(fd, parsedArgument[1][i], plusMode, parsedArgument);
    ++i;
  }
}

// ======== MODE <prefix><channel>
// ============================================= Replies:
// -- 324 RPL_CHANNELMODEIS:
// :<server> 324 <client> <channel> <modestring> <mode arguments>...
// e.g. :irc.example.com 324 Alice #chatroom +itkl secretpass 25

// -- 329_RPL_CREATIONTIME:
// creation time of a channel
// :<server> 329 <client> <channel> <creationtime>
// :irc.example.com 329 Alice #chatroom 1698544800
// <creationtime> is a unix timestamp representing when the channel was
// created

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

   When using the 'o' and 'b' options, a restriction on a total of three
   per mode command has been imposed.  That is, any combination of 'o'
   and

4.2.3.2 User modes

   Parameters: <nickname> {[+|-]|i|w|s|o}

   The user MODEs are typically changes which affect either how the
   client is seen by others or what 'extra' messages the client is sent.
   A user MODE command may only be accepted if both the sender of the
   message and the nickname given as a parameter are both the same.

   The available modes are as follows:

           i - marks a users as invisible;
           s - marks a user for receipt of server notices;
           w - user receives wallops;
           o - operator flag.

   Additional modes may be available later on.

   If a user attempts to make themselves an operator using the "+o"
   flag, the attempt should be ignored.  There is no restriction,
   however, on anyone `deopping' themselves (using "-o").  Numeric
   Replies:

           ERR_NEEDMOREPARAMS              RPL_CHANNELMODEIS
           ERR_CHANOPRIVSNEEDED            ERR_NOSUCHNICK
           ERR_NOTONCHANNEL                ERR_KEYSET
           RPL_BANLIST                     RPL_ENDOFBANLIST
           ERR_UNKNOWNMODE                 ERR_NOSUCHCHANNEL

           ERR_USERSDONTMATCH              RPL_UMODEIS
           ERR_UMODEUNKNOWNFLAG

   Examples:

           Use of Channel Modes:

MODE #Finnish +im               ; Makes #Finnish channel moderated and
                                'invite-only'.

MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on
                                channel #Finnish.

MODE #Finnish +v Wiz            ; Allow WiZ to speak on #Finnish.

MODE #Fins -s                   ; Removes 'secret' flag from channel
                                #Fins.

MODE #42 +k oulu                ; Set the channel key to "oulu".

MODE #eu-opers +l 10            ; Set the limit for the number of users
                                on channel to 10.

MODE &oulu +b                   ; list ban masks set for channel.

MODE &oulu +b *!*@*             ; prevent all users from joining.

MODE &oulu +b *!*@*.edu         ; prevent any user from a hostname
                                matching *.edu from joining.
        Use of user Modes:

:MODE WiZ -w                    ; turns reception of WALLOPS messages
                                off for WiZ.

:Angel MODE Angel +i            ; Message from Angel to make themselves
                                invisible.

MODE WiZ -o                     ; WiZ 'deopping' (removing operator
                                status).  The plain reverse of this
                                command ("MODE WiZ +o") must not be
                                allowed from users since would bypass
                                the OPER command.

*/
