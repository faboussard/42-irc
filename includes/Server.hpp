/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/29 16:43:57 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


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
#include <string>
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
    channelsMap _channels;
    struct sockaddr_in _address;
    std::vector<struct pollfd> _pollFds;

public:
    explicit Server(int port, const std::string &password); // Ensure password is passed as const ref
    #ifdef TEST
    Server() {};
    #endif
    /* Getters */
    int getSocketFd() const;
    int getPort() const;
    const std::string &getPassword() const;
    const Client &getClientByFd(int fd) const;
    const Channel &getChannelByName(const std::string &name) const;
    const channelsMap &getChannels() const;
    const clientsMap &getClients() const;
    const std::string &getServerName() const;

    /* Setters */
    void setStartTime();

    /* Clients Management */
    void acceptNewClient();
    void addClient(int fd, const Client &client);
    void sendConnectionMessage(const Client &client) const;
    void receiveMessage(int fd);
    void handleClientMessage(int fd);

    /* Clear and Close */
    void closeServer();
    void clearClient(int fd);
    void closeClient(int fd);
  /* Server Mounting */
  void runServer();
  void createSocket();
  void createPoll();
  std::string fetchStartTime();
  void acceptAndChat();
  static void signalHandler(int signal);

    /*--------- Commands Management --------------*/
    /* Join */
    void handleCommand(std::string &command, std::string &param, int fd);
    void joinChannel(std::string &channelName, int fd);
    void executeJoin(int fd, const Client &client, const std::string &channelName);
    
    // Sub-functions for joinChannel
    void createChannelIfNotExist(const std::string &channelName);
    void sendJoinMessageToClient(int fd, const std::string &nick, const std::string &channelName);
    void broadcastJoinMessage(int fd, const std::string &nick, const std::string &channelName) ;

    // Other methods
    void sendToAllClients(const std::string &message);
    void handlePassword(int fd);
};

#endif  // INCLUDES_SERVER_HPP_