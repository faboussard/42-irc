#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include <signal.h>

#include "Client.hpp"

class Client;

class Server {
private:
    static bool _signal;
    int _socketFd;
    int _port;
    std::vector<Client> _clients;
    struct sockaddr_in _address;
    std::vector<struct pollfd> _poll_fds;

public:
    Server(int port);
    void runServer();
    static void signalHandler(int signal);
    void closeServer();
    void createSocket();
    void clearClients(int fd);
    void acceptNewClient();
    void receiveMessage(int fd);
};

#endif // SERVER_HPP
