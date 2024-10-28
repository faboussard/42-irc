/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 10:57:41 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/28 11:16:07 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ======== PASS <password> ====================================================
// If the client is already registered, the server MUST ignore the PASS command
// and send 
// -- 462 ERR_ALREADYREGISTERED

// ======== PASS (without parameters) ==========================================
// Replies:
// -- 461 ERR_NEEDMOREPARAMS

// ======== PASS <wrong password> ==============================================
// Replies:
// -- 464 ERR_PASSWDMISMATCH
// and then close the connection with -- ERROR message (standard reply format)