/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/05 11:39:14 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/colors.hpp"
#include "../includes/numericReplies.hpp"
#include "../includes/utils.hpp"
#include "../includes/tests.hpp"

// to test: echo "\r\r\r\r\r" | nc 127.0.0.1 6667

void Test::testAll(Server &server) {
    std::cout << "\r\rJOIN" << std::endl;
    
    testJoinChannel(server);
    std::cout  << std::endl;

    std::cout << "All tests passed" << std::endl;
}