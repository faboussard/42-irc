<<<<<<< HEAD
/* MODE

 

MODE - Change the channel’s mode:
· i: Set/remove Invite-only channel
· t: Set/remove the restrictions of the TOPIC command to channel
operators
· k: Set/remove the channel key (password)
· o: Give/take channel operator privilege
5
ft_irc Internet Relay Chat
· l: Set/remove the user limit to channel


If the client cannot join this channel because they would be over their limit, they will receive an ERR_TOOMANYCHANNELS (405) reply and the command will fail.

*/
=======
/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 10:02:17 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/29 14:18:02 by yusengok         ###   ########.fr       */
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
>>>>>>> origin/send_numeric_messages
