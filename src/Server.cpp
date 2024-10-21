#include "Server.hpp"

bool Server::signal = false;

Server::Server(int port){
    this->port = port;
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(this->server_fd == 0){
        throw std::runtime_error("Socket creation failed");
    }
    int opt = 1;
    if(setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        throw std::runtime_error("Setsockopt failed");
    }
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(port);
    if(bind(this->server_fd, (struct sockaddr *)&this->address, sizeof(this->address)) < 0){
        throw std::runtime_error("Bind failed");
    }
    if(listen(this->server_fd, 3) < 0){
        throw std::runtime_error("Listen failed");
    }
    this->poll_fds.push_back({this->server_fd, POLLIN, 0});
}

