#ifndef INCLUDES_SERVER_HPP_
#define INCLUDES_SERVER_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include "../includes/numericReplies.hpp"

typedef std::map<int, Client> clientsMap;
typedef std::map<std::string, Channel> channelsMap;

class Server {
 private:
  static bool _signal;
  int _socketFd;
  int _port;
  std::string _name;
  std::string _startTime;
  std::string _password;
  clientsMap _clients;
  struct sockaddr_in _address;
  std::vector<struct pollfd> _pollFds;
  channelsMap _channels;

 public:
  explicit Server(int port, std::string password);

  /* Getters */

  int getSocketFd() const;
  int getPort() const;
  const std::string &getPassword() const;
  const Client &getClientByFd(int fd) const;
  Channel &getChannelByName(const std::string &name);
  const std::string &getServerName() const;
  std::string getChannelSymbol(const std::string &channelName) const;
  std::string getUserPrefix(const Client &client) const;

  /* Setters */
  void setStartTime();

  /* Server Mounting */
  void runServer();
  void createSocket();
  void createPoll();
  void acceptAndChat();
  static void signalHandler(int signal);

  /* Clients Management */

  void acceptNewClient();
  void sendConnectionMessage(const Client &client) const;
  void receiveMessage(int fd);
  void handleClientMessage(int fd);

  /* Clear and Close */

  void closeServer();
  void clearClient(int fd);
  void closeClient(int fd);

  /*--------- Commands Management --------------*/

  /* Join */
  void handleCommand(std::string &command, std::string &param, int fd);
  void joinChannel(std::string &channelName, int fd);

  // Sous-fonctions pour joinChannel
  std::string &prepareChannelName(std::string &channelName);
  void createChannelIfNotExist(const std::string &channelName);
  void sendJoinMessageToClient(int fd, const std::string &nick, const std::string &channelName);
  void sendNameReply(int fd, const std::string &nick, const std::string &channelName);
  void sendEndOfNames(int fd, const std::string &nick, const std::string &channelName);
  void broadcastJoinMessage(int fd, const std::string &nick, const std::string &channelName);

  // Autres méthodes
  void sendToAllClients(const std::string &message);
  void handlePassword(int fd);
};

#endif  // INCLUDES_SERVER_HPP_
