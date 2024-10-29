/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testJoinChannel.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/29 14:23:51 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <string>
#include "Server.hpp" 
#include "tests.hpp"

void testJoinChannel(Server &server) {
    
        int fd1 = 1;
    int fd2 = 2;
    
    Test jointest;
    Client newClient1(fd1, "Nick1");
    Client newClient2(fd2, "Nick2");



    server.addClient(fd1, newClient1);
    server.addClient(fd2, newClient2);

    std::string channel1 = "#channelA";
    std::string channel2 = "#channelB";
    std::string channel2 = "channelC";
    std::string channel2 = "&channelB";


    
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
    std::string channel3 = "#channelC";
    server.joinChannel(channel3, fd1);  // Cette tentative doit être rejetée si le nombre de canaux est limité

    // Test de JOIN 0 pour quitter tous les canaux
    std::cout << "Testing JOIN 0 to leave all channels" << std::endl;
    std::string leaveAll = "0";
    server.joinChannel(leaveAll, fd1);  // Simule une sortie de tous les canaux

    // Résultats
    std::cout << "Test completed." << std::endl;
}
