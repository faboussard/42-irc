/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   joinTest.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/05 18:27:44 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream> 
#include "../includes/Server.hpp"
#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"
#include "../includes/tests.hpp"


void Server::addClient(int fd, const Client& client) {
    if (_clients.find(fd) != _clients.end()) {
        throw std::runtime_error("Client already exists with the same file descriptor");
    }
    _clients[fd] = client;
#ifdef DEBUG
    std::cout << "Client added with fd " << fd << " and nickname " << client.getNickname() << std::endl;
#endif
}


std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void Test::testJoinChannel(Server &server) {
    
    int fd1 = 1;
    int fd2 = 2;
    int fd3 = 3;

    
    Client newClient1(fd1, "Nick1");
    Client newClient2(fd2, "Nick2");
    Client newClient3(fd3, "Nick3");


    server.addClient(fd1, newClient1);
    server.addClient(fd2, newClient2);
    server.addClient(fd3, newClient3);


    std::string channel1 = "#channel1";
    std::string channel2 = "#channel2";
    std::string channel3 = "#channel3";
    std::string channel4 = "#channel4";

    std::string channel5 = "#channel5";

    std::string channel6 = "#channel6";

    std::string channel7 = "#channel7";

    std::string channel8 = "#channel8";

    std::string channel9 = "#channel9";

    std::string channel10 = "#channel10";

    std::string channel11 = "#channel11";


    // Test de l'entrée d'un client dans un canal
    std::cout << MAGENTA "Testing client joining channel: " RESET << channel1 << std::endl;
    server.joinChannel(channel1, fd1, newClient1);

    // Test de l'entrée du deuxième client dans le même canal
    std::cout << MAGENTA "Testing second client joining the same channel: " RESET << channel1 << std::endl;
    server.joinChannel(channel1, fd2, newClient2);

    // Test de l'entrée du premier client dans un autre canal
    std::cout << MAGENTA "Testing first client joining another channel: " RESET << channel2 << std::endl;
    server.joinChannel(channel2, fd1, newClient1);

    // Test de la limite de canaux si une limite est implémentée (par exemple, 2 canaux max)
    std::cout << MAGENTA "Testing channel limit" RESET << std::endl;
    for (int i = 1; i <= 10; i++)
    {
        std::string channel = "#channel" + intToString(i);
        std::cout << MAGENTA "Testing client joining channel: " RESET << channel << std::endl;
        server.joinChannel(channel, fd3, newClient3);
    }
    std::cout << "fd3 is in " << newClient3.getChannelsCount() << " channels" << std::endl;
    server.joinChannel(channel11, fd3, newClient3);  // Cette tentative doit être rejetée si le nombre de canaux est limité
    std::cout << "fd3 is in " << newClient3.getChannelsCount() << " channels" << std::endl;

    // Test de JOIN 0 pour quitter tous les canaux
    std::cout << MAGENTA "Testing JOIN 0 to leave all channels" RESET << std::endl;
    std::string leaveAll = "0";
    server.joinChannel(leaveAll, fd1, newClient1);  // Simule une sortie de tous les canaux

    // Test de JOIN joindre plusieurs canaux en meme temps
    std::cout << MAGENTA "Testing JOIN multiple channels" RESET << std::endl;
    std::string multipleChannels = "#channelA,#channelB";
    server.joinChannel(multipleChannels, fd1, newClient1);  // Simule une sortie de tous les canaux
    
    // bad param
    std::cout << MAGENTA "Testing JOIN bad prefix" RESET << std::endl;
    std::string badParam = "&ssssss";
    server.joinChannel(badParam, fd1, newClient1); 

    // no param
    std::cout << MAGENTA "Testing JOIN no prefix" RESET << std::endl;
    std::string noprefix = "g";
    server.joinChannel(noprefix, fd1, newClient1); 

    // empty param
    std::cout << MAGENTA "Testing JOIN empty param" RESET << std::endl;
    std::string empty = "";
    server.joinChannel(empty, fd1, newClient1); 


// WHAT IS EXPECTED
    // # param
    std::cout << MAGENTA "Testing JOIN # param" RESET << std::endl;
    std::string diez = "#";
    server.joinChannel(diez, fd1, newClient1); 

        // # param
    std::cout << MAGENTA "Testing JOIN # param" RESET << std::endl;
    std::string diezSpace = "#a #g";
    server.joinChannel(diezSpace, fd1, newClient1); 

    // Résultats
    std::cout << "Test completed." << std::endl;
}
