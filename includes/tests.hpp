/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 09:01:51 by mbernard          #+#    #+#             */
/*   Updated: 2024/11/05 11:20:38 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_TESTS_HPP_
#define INCLUDES_TESTS_HPP_

#include <sstream>
#include <string>
#include <vector>

#include "Server.hpp"

class Test
{
    public:
    void testAll(Server &server);
    
    /* join */
    void testJoinChannel(Server &server);
    
    
};

#endif  //  INCLUDES_TESTS_HPP_
