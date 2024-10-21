#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client {
public:
    Client();
    int _socket;
    std::string _ip;
};

#endif // CLIENT_HPP
