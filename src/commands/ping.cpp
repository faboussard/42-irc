/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 15:09:10 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/29 15:12:45 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ======== PING <server or timestamp> =========================================
// Replies:
// PONG <server or timestamp>

// ======== PING (without parameters) ==========================================
// *** Hexchat autonmatically adds timestamp when we send /PING
// netcat doesn't add anything with /PING
// -- 409 ERR_NOORIGIN