/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 11:50:56 by faboussa          #+#    #+#             */
/*   Updated: 2024/10/22 17:02:31 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

#include "../includes/colors.hpp"


Client::Client(int fd, const std::string& ip) : _fd(fd), _ip(ip) {}

void Client::sendMessage(const std::string& message) {
  if (send(_fd, message.c_str(), message.length(), 0) == -1) {
    std::cerr << RED "Error while sending message" RESET << std::endl;
  }
}
