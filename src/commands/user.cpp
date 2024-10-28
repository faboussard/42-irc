/* Copyright 2024 <yusengok> ************************************************ */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yusengok <yusengok@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 11:14:50 by yusengok          #+#    #+#             */
/*   Updated: 2024/10/28 11:55:10 by yusengok         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// ======== USER <parametres> ==================================================
// If the client is already registered, the server MUST ignore the USER command
// and send 
// -- 462 ERR_ALREADYREGISTERED

// ======== USER (insufficiant parameters or empty parameter) ==================
// Replies:
// -- 461 ERR_NEEDMOREPARAMS