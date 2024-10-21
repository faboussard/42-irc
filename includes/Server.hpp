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

class Server {
public:
    Server(int port);
    ~Server();
    void runServer();
    static void signalHandler(int signal);
    void closeServer();

private:
    static bool signal;
    int server_fd;
    int port;
    struct sockaddr_in address;
    std::vector<struct pollfd> poll_fds;

    // void handleClient(int client_socket);

};

#endif // SERVER_HPP
