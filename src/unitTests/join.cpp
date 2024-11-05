/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/05 11:53:07 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/Server.hpp"
#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"
#include "../includes/tests.hpp"

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
    std::cout << MAGENTA "Testing client joining channel: " RESET << channel1 << std::endl;
    server.joinChannel(channel1, fd1);

    // Test de l'entrée du deuxième client dans le même canal
    std::cout << MAGENTA "Testing second client joining the same channel: " RESET << channel1 << std::endl;
    server.joinChannel(channel1, fd2);

    // Test de l'entrée du premier client dans un autre canal
    std::cout << MAGENTA "Testing first client joining another channel: " RESET << channel2 << std::endl;
    server.joinChannel(channel2, fd1);

    // Test de la limite de canaux si une limite est implémentée (par exemple, 2 canaux max)
    std::cout << MAGENTA "Testing channel limit" RESET << std::endl;
    std::cout << "Chanlimit is " << CHANLIMIT_ << std::endl;
    std::cout << "fd1 is in " << server.getClientByFd(fd1).getChannelsCount() << " channels" << std::endl;
    server.joinChannel(channel3, fd1);  // Cette tentative doit être rejetée si le nombre de canaux est limité
    std::cout << "fd1 is in " << server.getClientByFd(fd1).getChannelsCount() << " channels" << std::endl;

    // Test de JOIN 0 pour quitter tous les canaux
    std::cout << MAGENTA "Testing JOIN 0 to leave all channels" RESET << std::endl;
    std::string leaveAll = "0";
    server.joinChannel(leaveAll, fd1);  // Simule une sortie de tous les canaux

    // bad param
    std::cout << MAGENTA "Testing JOIN bad prefix" RESET << std::endl;
    std::string badParam = "&ssssss";
    server.joinChannel(badParam, fd1); 

    // no param
    std::cout << MAGENTA "Testing JOIN no prefix" RESET << std::endl;
    std::string noprefix = "g";
    server.joinChannel(noprefix, fd1); 

    // empty param
    std::cout << MAGENTA "Testing JOIN empty param" RESET << std::endl;
    std::string empty = "";
    server.joinChannel(empty, fd1); 

    // Résultats
    std::cout << "Test completed." << std::endl;
}
