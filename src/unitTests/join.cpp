/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/05 11:42:01 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <string>
#include "Server.hpp" 
#include "tests.hpp"

void Test::testJoinChannel(Server &server) {
    
    int fd1 = 1;
    int fd2 = 2;
    
    Client newClient1(fd1, "Nick1");
    Client newClient2(fd2, "Nick2");

    server.addClient(fd1, newClient1);
    server.addClient(fd2, newClient2);

    std::string channel1 = "#channelA";
    std::string channel2 = "#channelB";
    std::string channel3 = "#channelC";

    // Test de l'entrée d'un client dans un canal
    std::cout << "Testing client joining channel: " << channel1 << std::endl;
    server.joinChannel(channel1, fd1);

    // Test de l'entrée du deuxième client dans le même canal
    std::cout << "Testing second client joining the same channel: " << channel1 << std::endl;
    server.joinChannel(channel1, fd2);

    // Test de l'entrée du premier client dans un autre canal
    std::cout << "Testing first client joining another channel: " << channel2 << std::endl;
    server.joinChannel(channel2, fd1);

    // Test de la limite de canaux si une limite est implémentée (par exemple, 2 canaux max)
    std::cout << "Testing channel limit" << std::endl;
    std::cout << "Chanlimit is " << CHANLIMIT_ << std::endl;
    std::cout << "fd1 is in " << server.getClientByFd(fd1).getChannelsCount() << " channels" << std::endl;
    server.joinChannel(channel3, fd1);  // Cette tentative doit être rejetée si le nombre de canaux est limité
    std::cout << "fd1 is in " << server.getClientByFd(fd1).getChannelsCount() << " channels" << std::endl;

    // Test de JOIN 0 pour quitter tous les canaux
    std::cout << "Testing JOIN 0 to leave all channels" << std::endl;
    std::string leaveAll = "0";
    server.joinChannel(leaveAll, fd1);  // Simule une sortie de tous les canaux

    // bad param
    std::cout << "Testing JOIN bad prefix" << std::endl;
    std::string badParam = "&ssssss";
    server.joinChannel(badParam, fd1); 

    // no param
    std::cout << "Testing JOIN no prefix" << std::endl;
    std::string noprefix = "g";
    server.joinChannel(noprefix, fd1); 

    // empty param
    std::cout << "Testing JOIN empty param" << std::endl;
    std::string empty = "";
    server.joinChannel(empty, fd1); 

    // Résultats
    std::cout << "Test completed." << std::endl;
}
