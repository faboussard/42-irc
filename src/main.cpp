
/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/17 11:53:10 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Colors.hpp"
#include "../includes/Server.hpp"

void checkArgs(int port, std::string password){
    if(port < 1080 || port > 65535){
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
      Server ser(port);

    try
    {
      signal(SIGINT, Server::signalHandler);
      signal(SIGQUIT, Server::signalHandler);
      // ser.runServer();
      std::cout << GREEN << "Server started on port " << port << RESET << std::endl;
      
    }
    catch(const std::exception& e)
    {
        ser.closeServer();
        std::cerr << e.what() << '\n';
    }
    std::cout << "The Server Closed!" << std::endl;
    exit(EXIT_SUCCESS);
}
