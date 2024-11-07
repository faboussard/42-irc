/* Copyright 2024 <faboussa>************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: faboussa <faboussa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 10:02:17 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/29 18:01:25 by faboussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ======== MODE <prefix><channel> =============================================
// Replies:
// -- 324 RPL_CHANNELMODEIS: 
// :<server> 324 <client> <channel> <modestring> <mode arguments>...
// e.g. :irc.example.com 324 Alice #chatroom +itkl secretpass 25

// -- 329_RPL_CREATIONTIME:
// creation time of a channel
// :<server> 329 <client> <channel> <creationtime>
// :irc.example.com 329 Alice #chatroom 1698544800
// <creationtime> is a unix timestamp representing when the channel was created


// ======== MODE <prefix><channel> <modestring> <target nick> ==================
// e.g. MODE #channel +o <target nick>
// Replies:
// -- 441 ERR_USERNOTINCHANNEL:
// :<server> 441 <client> <target nick> <channel> :They aren't on that channel
// The target isn’t joined to the channel
