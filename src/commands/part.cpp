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
    #ifdef DEBUG
      std::cout << "Client " << client->getNickname() << " is leaving channel " << it->second->getName() << std::endl;
    #endif
    // Accéder à l'objet Channel à partir de la map (c'est un pointeur)
    Channel *channel = it->second;

    sendPartMessageToClient(fd, client->getNickname(), it->second->getName());

    broadcastPartMessage(fd, client->getNickname(), it->second->getName());
        channel->removeClientFromTheChannel(fd);
    client->decrementChannelsCount(); 
  }
}


void Server::broadcastPartMessage(int fd, const std::string &nick, const std::string &channelName) {
    std::string partMessage = ":" + nick + " PART :#" + channelName + "\r\n";
    
    // On récupère une référence constante à la map des clients dans le canal
    clientPMap clientsInChannel = getChannelByName(channelName).getChannelClients();
    
    // Utilisation d'un const_iterator pour itérer sur une map constante
    for (clientPMap::iterator it = clientsInChannel.begin(); it != clientsInChannel.end(); ++it) {
        if (it->first != fd) {  // Ne pas envoyer au client qui quitte
            if (send(it->first, partMessage.c_str(), partMessage.length(), 0) == -1) {
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
