#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client {
private:
    int _fd;
    std::string _ip;

public:
    Client() : _fd(-1) {}
    int getFd() const { return _fd; }
    void setFd(int fd) { _fd = fd; }

    std::string getIp() const { return _ip; }
    void setIp(const std::string& ip) { _ip = ip; }
};

#endif // CLIENT_HPP
