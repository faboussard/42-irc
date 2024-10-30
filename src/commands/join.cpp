/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   joinChannel.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/29 17:50:06 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"

/* join several channels */
// Servers MUST process the parameters of this command as lists on incoming
// commands from clients, with the first <key>
//  being used for the first <channel>, the second <key> being used for the
//  second <channel>, etc.

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

//gerer que on q #pqrqm pui si virgule, on doit fqire join sur le deuxieme #param. si espqce -> key.
bool Server::canEnterChannel(const std::string &param) {
  return (                 // Ensure param length is greater than 1
      param.length() < 51  // Channel name should be less than 51 characters
      && param.find(" ") == std::string::npos  // No spaces in param
      &&
      param[0] ==
          CHAN_PREFIX_CHAR);  // Check if first character is the channel prefix
}

void Server::joinChannel(std::string &param, int fd) {
  std::string channelName;
  channelName = param.substr(1);
  createChannelIfNotExist(channelName);
#ifdef DEBUG
  std::cout << GREY "CHANNEL NAME " << channelName << " param " << param
            << RESET << std::endl;
  std::cout.flush();
#endif
  const Client &client = getClientByFd(fd);
  _channels[channelName].acceptClientInTheChannel(client);
  executeJoin(fd, client, channelName);
}

void Server::executeJoin(int fd, const Client &client,
                         const std::string &channelName) {
  std::string nick = client.getNickName();
  sendJoinMessageToClient(fd, nick, channelName);
  send353Namreply(fd, nick, _channels[channelName]);
  send366Endofnames(fd, nick, channelName);
  broadcastJoinMessage(fd, nick, channelName);
}

void Server::createChannelIfNotExist(const std::string &channelName) {
  if (_channels.find(channelName) == _channels.end()) {
    Channel newChannel(channelName);
    _channels[channelName] = newChannel;
  }
}

void Server::sendJoinMessageToClient(int fd, const std::string &nick,
                                     const std::string &channelName) {
  std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
  send(fd, joinMessage.c_str(), joinMessage.length(), 0);
#ifdef DEBUG
  std::cout << GREY "Client " << nick << " joined channel " RESET << channelName
            << std::endl;
#endif
  send331Notopic(fd, nick, _channels[channelName]);
}

void Server::broadcastJoinMessage(int fd, const std::string &nick,
                                  const std::string &channelName) {
  std::string joinMessage = ":" + nick + " JOIN :" + channelName + "\r\n";
  const clientsMap &clientsInChannel =
      _channels[channelName].getClientsInChannel();
  for (clientsMap::const_iterator it = clientsInChannel.begin();
       it != clientsInChannel.end(); ++it) {
    if (it->first != fd) {
      send(it->first, joinMessage.c_str(), joinMessage.length(), 0);
    }
  }
}