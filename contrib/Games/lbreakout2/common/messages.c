/***************************************************************************
                          msg.h  -  description
                             -------------------
    begin                : Mon Oct 21 12:02:57 CEST 2002
    copyright            : (C) 2001 by Michael Speck
    email                : kulkanie@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef NETWORK_ENABLED

#include "net.h"
#include "messages.h"

/* Define messages */
void net_init_msgs( void )
{
    net_set_msg( MSG_ERROR, "z" );
    net_set_msg( MSG_SERVER_INFO,  "z" );
    net_set_msg( MSG_IDENTIFY, "bz" );
    net_set_msg( MSG_LOGIN_OKAY, "szz" );
    net_set_msg( MSG_CHAT, "z" );
    net_set_msg( MSG_ADD_USER, "sz" );
    net_set_msg( MSG_REMOVE_USER, "s" );
    net_set_msg( MSG_ADD_USERS, "br" );
    net_set_msg( MSG_ADD_GAME, "szzz" );
    net_set_msg( MSG_REMOVE_GAME, "s" );
    net_set_msg( MSG_UPDATE_GAME, "sb" );
    net_set_msg( MSG_ADD_GAMES, "br" );
    net_set_msg( MSG_ADD_CHANNEL, "sz" );
    net_set_msg( MSG_REMOVE_CHANNEL, "s" );
    net_set_msg( MSG_ADD_CHANNELS, "br" );
    net_set_msg( MSG_CHALLENGE, "sszbbbb" );
    net_set_msg( MSG_IP_ADDRESS, "szi" );
    net_set_msg( MSG_LEVEL_DATA, "zzrr" );
    net_set_msg( MSG_BENCHMARK, "ss" );
    net_set_msg( MSG_WINNER, "b" );
    net_set_msg( MSG_ENTER_CHANNEL, "z" );
    net_set_msg( MSG_SET_CHANNEL, "s" );
    net_set_msg( MSG_INIT_TRANSFER, "szbbb" );
    net_set_msg( MSG_DM_HOSTDATA, "bbsr" );
    net_set_msg( MSG_DM_CLIENTDATA, "s" );
    net_set_msg( MSG_MP_HOSTDATA, "iisiirrr" );
    net_set_msg( MSG_MP_CLIENTDATA, "s" );
    net_set_msg( MSG_WHISPER, "sz" );
    net_set_msg( MSG_OPEN_TRANSFER, "s" );
    net_set_msg( MSG_BUSY, "s" );
    net_set_msg( MSG_CANCEL_CHALLENGE, "s" );
    net_set_msg( MSG_REJECT_CHALLENGE, "s" );
    net_set_msg( MSG_ACCEPT_CHALLENGE, "s" );
    net_set_msg( MSG_OLDER_SET, "s" );
    net_set_msg( MSG_CANCEL_TRANSFER, "s" );
    net_set_msg( MSG_REJECT_SET, "s" );
    net_set_msg( MSG_ACCEPT_SET, "s" );
    net_set_msg( MSG_TRANSFER_DATA, "szzrr" );
    net_set_msg( MSG_ROUND_OVER, "bb" );
    net_set_msg( MSG_GAME_STATS, "sbbbbbssbbiibbbb" );
}

#endif
