#include "Server.hpp"

void checkArgs(int port, std::string password){
    if(port < 1024 || port > 65535){
        std::cerr << "Invalid port number" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(password.length() > 1000){
        std::cerr << "Password too long" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int ac, char **argv){
    std::string usage = "Usage: ./server <port>";
    if(ac != 2){
        std::cerr << usage << std::endl;
        exit(EXIT_FAILURE);
    }

    int port = std::atoi(argv[1]);
    std::string password = argv[2];
    checkArgs(port, password);

    // Server ser(port);
    // try
    // {
    //     signal(SIGINT, Server::signalHandler);
    //     signal(SIGQUIT, Server::signalHandler);
    //     ser.runServer();
    //     std::cout << "Server is running..." << std::endl;
    // }
    // catch(const std::exception& e)
    // {
    //     ser.closeServer();
    //     std::cerr << e.what() << '\n';
    // }
    // std::cout << "Server is Closed ! " << std::endl;
    // return 0;
}