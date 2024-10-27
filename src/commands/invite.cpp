/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 14:48:41 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/27 21:55:48 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ======== INVITE (without parameters) ========================================
// Replies:
// -- 336 RPL_INVITELIST:
// Indicate a channel the client was invited to. 
// If the client is invited to multiple channels, the server sends multiple replies.
// If there is no channel to which the client was invited, the server sends only 337.

// -- 337 RPL_ENDOFINVITELIST:

// ======= INVITE <nick of the person to be invited> <channel name> ============
// Replies:
// -- 341 RPL_INVITING:
// Confirms the user has been invited to the channel.

// -- 403 ERR_NOSUCHCHANNEL:
// The channel does not exist.

// -- 442 ERR_NOTONCHANNEL:
// Only members of the channel are allowed to invite other users. 
// Otherwise, the server MUST reject the command with 442.

// -- 443 ERR_USERONCHANNEL:
// The user is already on the target channel, the server MUST reject with 443.

// -- 482 ERR_CHANOPRIVSNEEDED:
// The user is not a channel operator.

// ======= INVITE (without not enough parameters) ==============================
// Replies:
// -- 461 ERR_NEEDMOREPARAMS:
