#include "Server.hpp"

bool Server::_signal = false;

Server::Server(int port) : _port(port){
    _signal = false;
    _socketFd = -1;
}

void Server::runServer(){

    std::cout << "Server running" << std::endl;
}

void Server::closeServer(){
    if (_socketFd != -1)
    {
        close(_socketFd);
        std::cout << "Server closed" << std::endl;
    }
}

void Server::signalHandler(int signal){
    if (signal == SIGINT || signal == SIGQUIT)
    {
        Server::_signal = true;
    }
}

//revoir
void Server::clearClients(int fd){ //-> clear the clients
	for(size_t i = 0; i < _poll_fds.size(); i++){ //-> remove the client from the pollfd
		if (_poll_fds[i].fd == fd)
			{_poll_fds.erase(_poll_fds.begin() + i); break;}
	}
	for(size_t i = 0; i < _clients.size(); i++){ //-> remove the client from the vector of clients
		if (_socketFd == fd)
			{_clients.erase(_clients.begin() + i); break;}
	}

}

void Server::createSocket(){
    _socketFd = socket(AF_INET, SOCK_STREAM, 0); // ipv4, tcp, 0
    if (_socketFd == -1)
    {
        throw std::runtime_error("Failed to create socket");
    }
    _address.sin_family = AF_INET; //An integer representing the address family. For IPv4
    _address.sin_addr.s_addr = INADDR_ANY; // It represents “any” IP address, meaning the socket will be bound to all available network interfaces on the host.
    _address.sin_port = htons(_port); //converting a 16-bit unsigned short integer from host byte order to network byte order.
    if (bind(_socketFd, (struct sockaddr *)&_address, sizeof(_address)) == -1)
    {
        throw std::runtime_error("Failed to bind socket");
    }
    if (listen(_socketFd, 10) == -1) // int listen(int sockfd, int backlog);
    {
        throw std::runtime_error("Failed to listen on socket");
    }
   _poll_fds.push_back({_socketFd, POLLIN, 0}); //  fill the struct pollfd with the server information. To make the server ready to receive incoming connections, set the events argument to POLLIN and push the struct to the vector fds.
}