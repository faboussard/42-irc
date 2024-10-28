/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   joinChannel.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/28 15:05:51 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

#include "../includes/colors.hpp"
#include "../includes/utils.hpp"
#include "../includes/numericReplies.hpp"


/* join several channels */
//Servers MUST process the parameters of this command as lists on incoming commands from clients, with the first <key>
// being used for the first <channel>, the second <key> being used for the second <channel>, etc.

/* join an existing channel */

/*
While a client is joined to a channel, 
they receive all relevant information about that channel including the 
JOIN, PART, KICK, and MODE messages affecting the channel. 
They receive all PRIVMSG and NOTICE messages sent to the
 channel, and they also receive QUIT messages from other clients 
 joined to the same channel (to let them know those users have left
  the channel and the network). This allows them to keep track of other
   channel members and channel modes.*/

/*
  tackle the channels modes. 

*/

/* 
JOIN 0 = part all channels
*/

/* 
- restrict the number of channels a user can join to CHANLIMIT.
*/





void Server::joinChannel(std::string &param, int fd) {
    std::string channelName;
    channelName = prepareChannelName(param);
    createChannelIfNotExist(channelName);
    // #ifdef DEBUG
        std::cout << GREY "CHANNEL NAME " << channelName << " param " << param << RESET << std::endl;
        std::cout.flush();
    // #endif
    const Client &client = getClientByFd(fd);
    _channels[channelName].acceptClientInTheChannel(client);

    std::string nick = client.getNickName();
    sendJoinMessageToClient(fd, nick, channelName);
    sendNameReply(fd, nick, channelName);
    sendEndOfNames(fd, nick, channelName);
    broadcastJoinMessage(fd, nick, channelName);
}

std::string &Server::prepareChannelName(std::string &channelName) {
    channelName = channelName.substr(1);
}

void Server::createChannelIfNotExist(const std::string &channelName) {
    if (_channels.find(channelName) == _channels.end()) {
        Channel newChannel(channelName);
        _channels[channelName] = newChannel;
    }
}

void Server::sendJoinMessageToClient(int fd, const std::string &nick, const std::string &channelName) {
    std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
    send(fd, joinMessage.c_str(), joinMessage.length(), 0);
    #ifdef DEBUG
    std::cout "hahaha" << std::endl;
      std::cout << GREY "Client " << nick << " joined channel " RESET << channelName << std::endl;
    #endif
    // ajouter condition if topic 
    send331Notopic(fd, nick, _channels[channelName]);
}

void Server::sendNameReply(int fd, const std::string &nick, const std::string &channelName) {
    std::string namesList;
    const clientsMap &clientsInChannel = _channels[channelName].getClientsInChannel();
    
    for (clientsMap::const_iterator it = clientsInChannel.begin(); it != clientsInChannel.end(); ++it) {
        std::string prefix = getUserPrefix(it->second); // Fonction pour obtenir le préfixe en fonction du statut (opérateur, etc.)
        namesList += prefix + getClientByFd(it->first).getNickName() + " ";
    }
    if (!namesList.empty()) {
        namesList.pop_back(); // Retirer l’espace final
    }

    std::string symbol = getChannelSymbol(channelName); // Fonction pour obtenir le symbole de statut du canal
    std::string nameReply = _353_RPL_NAMREPLY(nick, symbol, channelName, namesList);
    send(fd, nameReply.c_str(), nameReply.length(), 0);
}

void Server::sendEndOfNames(int fd, const std::string &nick, const std::string &channelName) {
    std::string endOfNamesMessage = _366_RPL_ENDOFNAMES(nick, channelName); // utiliser numeric reply yuko
    send(fd, endOfNamesMessage.c_str(), endOfNamesMessage.length(), 0);
}

void Server::broadcastJoinMessage(int fd, const std::string &nick, const std::string &channelName) {
    std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
    const clientsMap &clientsInChannel = _channels[channelName].getClientsInChannel();
    for (clientsMap::const_iterator it = clientsInChannel.begin(); it != clientsInChannel.end(); ++it) {
        if (it->first != fd) {
            send(it->first, joinMessage.c_str(), joinMessage.length(), 0);
        }
    }
}


// void Server::handlePassword(int fd) {
//   char buffer[1024] = {0};
//   std::memset(buffer, 0, sizeof(buffer));
//   int valread = recv(fd, buffer, sizeof(buffer), 0);

//   switch (valread) {
//     case -1:
//       std::cerr << RED "Error while receiving message" RESET << std::endl;
//     case 0:
//       clearClient(fd);
//       return;
//   }
//   std::cout << YELLOW << buffer << RESET << std::endl;
//   std::string message(buffer, valread);
//   std::istringstream iss(message);
//   sendToAllClients(message);
// }

//----------------------------------  Channel Membership Prefixes ----------------------------------


// Channel Membership Prefixes
// Users joined to a channel may get certain privileges or status in that channel based on channel modes given to them. These users are given prefixes before their nickname whenever it is associated with a channel (ie, in NAMES, WHO and WHOIS messages). The standard and common prefixes are listed here, and MUST be advertised by the server in the PREFIX RPL_ISUPPORT parameter on connection.

// Founder Prefix
// This mode is used in a large number of networks. The prefix and mode letter typically used for it, respectively, are "~" and "+q".

// This prefix shows that the given user is the ‘founder’ of the current channel and has full moderation control over it – ie, they are considered to ‘own’ that channel by the network. This prefix is typically only used on networks that have the concept of client accounts, and ownership of channels by those accounts.

// Protected Prefix
// This mode is used in a large number of networks. The prefix and mode letter typically used for it, respectively, are "&" and "+a".

// Users with this mode cannot be kicked and cannot have this mode removed by other protected users. In some software, they may perform actions that operators can, but at a higher privilege level than operators. This prefix is typically only used on networks that have the concept of client accounts, and ownership of channels by those accounts.

// Operator Prefix
// This mode is standard. The prefix and mode letter used for it, respectively, are "@" and "+o".

// Users with this mode may perform channel moderation tasks such as kicking users, applying channel modes, and set other users to operator (or lower) status.

// Halfop Prefix
// This mode is widely used in networks today. The prefix and mode letter used for it, respectively, are "%" and "+h".

// Users with this mode may perform channel moderation tasks, but at a lower privilege level than operators. Which channel moderation tasks they can and cannot perform varies with server software and configuration.

// Voice Prefix
// This mode is standard. The prefix and mode letter used for it, respectively, are "+" and "+v".

// Users with this mode may send messages to a channel that is moderated.