/* Copyright 2024 <mbernard>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <mbernard@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/10/17 15:12:35 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include "../includes/Colors.hpp"


/* Les serveurs IRC (Internet Relay Chat) utilisent généralement le port 6667
pour les connexions non sécurisées (plaintext) et le port 6697 pour les 
connexions sécurisées (SSL/TLS). Cependant, il est possible de configurer un 
serveur IRC pour utiliser un port différent.

En ce qui concerne les valeurs minimales et maximales des ports, les ports
réseau valides sont compris entre 0 et 65535. 
Cependant, les ports de 0 à 1023 sont réservés aux services bien connus et 
il est généralement recommandé d'utiliser des ports au-dessus de 1023 pour
les applications personnelles afin d'éviter les conflits
*/

static bool isValidNumber(const std::string arg) {
  size_t size = arg.size();

  if (arg.empty() || arg[0] == '-') {
    std::cerr << RED "Error: invalid argument :" << std::endl;
    if (arg.empty())
      std::cerr << "Empty string !" RESET << std::endl;
    else
      std::cerr << arg << " is not a positive int number !" RESET << std::endl;//ERR_NOSUCHSERVER TO IMPLEMENT
    return (false);
  }
  for (size_t len = 0; len < size; len++) {
    if (!isdigit(arg[len])) {
      std::cerr << RED "Error: invalid argument :" << std::endl;
      std::cerr << arg << " is not composed of digits only !";
      std::cerr << RESET << std::endl;
      return (false);
    }
  }
  long long number = std::atoll(arg.c_str());
  if (number < 1023 || number > 65535) {
    std::cerr << RED "Error: invalid argument :" << std::endl; //ERR_NOSUCHSERVER TO IMPLEMENT
    if (number > 65535)
      std::cerr << arg << " is bigger than 65535 !" RESET << std::endl;
    else 
      std::cerr << arg << " is less than 65535 !" RESET << std::endl;
    return (false);
  }
  return (true);
}

static bool   check_args(int ac, char **av) {
  if (ac != 3) {
    std::cerr << RED "Error : wrong number of arguments" RESET << std::endl;//ERR_NEEDMOREPARAMS TO IMPLEMENT
    std::cerr << "Correct usage : ./ircserv [int] [string]" << std::endl;
    return (false);
  }
  if (!isValidNumber(av[0]))
    return (false);
  return (true);
}

int    main(int ac, char **av) {
  if (check_args(ac, av + 1))
    return (1);
  return (0);
}
