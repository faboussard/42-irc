#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <csignal>
#include "Client.hpp"

class Client;

class Server {
private:
    static bool _signal;
    int _socketFd;
    int _port;
    std::vector<Client> _clients;
    struct sockaddr_in _address;
    std::vector<struct pollfd> _pollFds;

public:
    Server(int port);

    static void signalHandler(int signal);

    void runServer();
    void closeServer();
    void createSocket();
    void clearClients(int fd);
    void acceptNewClient();
    void receiveMessage(int fd);
};

#endif // SERVER_HPP
