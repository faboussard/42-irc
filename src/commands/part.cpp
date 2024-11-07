#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"
void Server::quitChannel(int fd) {
  Client *client = &_clients.at(fd);

  // Retirer la constance de la map en la copiant dans une variable non constante
  channelPMap& mutableChannels = const_cast<channelPMap&>(client->getChannels());

  // Maintenant, on peut obtenir un iterator de la map mutable
  channelPMap::iterator itBegin = mutableChannels.begin();
  channelPMap::iterator itEnd = mutableChannels.end();

  for (channelPMap::iterator it = itBegin; it != itEnd; ++it) {
    Channel *channel = it->second;
    channel->removeClientFromTheChannel(fd);
    client->decrementChannelsCount();
    sendPartMessageToClient(fd, client->getNickname(), const_cast<channelPMap&>(getChannelByName(it->second)));
    broadcastPartMessage(fd, client->getNickname(), getChannelByName(it->second));
  }
}

void Server::sendJoinMessageToClient(int fd, const std::string &nick,
                                     const std::string &channelName,
                                     const Client &client) {
  std::string joinMessage = ":" + nick + " JOIN :#" + channelName + "\r\n";
  if (send(fd, joinMessage.c_str(), joinMessage.length(), 0) == -1)
    throw std::runtime_error(RUNTIME_ERROR);
  if (_channels[channelName].getTopic().topic.empty())
    send331Notopic(client, _channels[channelName]);
  else
    send332Topic(client, _channels[channelName]);
}


  void Server::broadcastPartMessage(int fd, const std::string &nick,
                                    const std::string &channelName) {
    std::string partMessage = ":" + nick + " PART :#" + channelName + "\r\n";
    std::map<int, Client *> clientsInChannel = getChannelByName(channelName).getChannelClients();
    std::map<int, Client *>::const_iterator it = clientsInChannel.begin();
    std::map<int, Client *>::const_iterator ite = clientsInChannel.end();

    for (; it != ite; ++it) {
      if (it->first != fd) {
        if (send(it->first, partMessage.c_str(), partMessage.length(), 0) ==
            -1) {
          throw std::runtime_error("Runtime error: send failed");
        }
      }
    }
  }

  void Server::sendPartMessageToClient(int fd, const std::string &nick, const std::string &channelName) {
  std::string partMessage = ":" + nick + " PART :#" + channelName + "\r\n";
  if (send(fd, partMessage.c_str(), partMessage.length(), 0) == -1) {
    throw std::runtime_error("Runtime error: send failed");
  }
}
