/* Emacs style mode select   -*- C++ -*- */
/*-----------------------------------------------------------------------------*/
/**/
/* $Id$*/
/**/
/* Copyright (C) 1993-1996 by id Software, Inc.*/
/**/
/* This source is available for distribution and/or modification*/
/* only under the terms of the DOOM Source Code License as*/
/* published by id Software. All rights reserved.*/
/**/
/* The source is distributed in the hope that it will be useful,*/
/* but WITHOUT ANY WARRANTY; without even the implied warranty of*/
/* FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License*/
/* for more details.*/
/**/
/* $Log$
 * Revision 1.1  2000/02/29 18:21:03  stegerg
 * Doom port based on ADoomPPC. Read README.AROS!
 **/
/**/
/* DESCRIPTION:*/
/**/
/*-----------------------------------------------------------------------------*/

static const char
rcsid[] = "$Id$";

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "i_system.h"
#include "d_event.h"
#include "d_net.h"
#include "m_argv.h"

#include "doomstat.h"

#ifdef __GNUG__
#pragma implementation "i_net.h"
#endif
#include "i_net.h"

#include "aros_netspecial.h"

/****************************************************************************/

extern arosnetdriver_t local_netdriver;

/****************************************************************************/

static arosnetdriver_t *netdrivers[] =
{
    &local_netdriver,
    NULL
};

static arosnetdriver_t *netdriver;

/****************************************************************************/

void I_InitNetwork (void)
{
    boolean ok;
    arosnetdriver_t	**drivers;
    int			i;
    	
    doomcom = malloc (sizeof (*doomcom) );
    memset (doomcom, 0, sizeof(*doomcom) );
    
    /* set up for network*/
    i = M_CheckParm ("-dup");
    if (i && i< myargc-1)
    {
	doomcom->ticdup = myargv[i+1][0]-'0';
	if (doomcom->ticdup < 1)
	    doomcom->ticdup = 1;
	if (doomcom->ticdup > 9)
	    doomcom->ticdup = 9;
    }
    else
	doomcom-> ticdup = 1;
	
    if (M_CheckParm ("-extratic"))
	doomcom-> extratics = 1;
    else
	doomcom-> extratics = 0;
		

    ok = false;
    
    for(drivers = netdrivers; *drivers; drivers++)
    {
        netdriver = *drivers;
        if ((i = M_CheckParm((char *)netdriver->activateparam)))
	{
	    ok = netdriver->InitNetwork();
	    break;
	}
    }
    
    if (!ok)
    {
        netdriver = NULL;
	
	/* single player game*/
	netgame = false;
	doomcom->id = DOOMCOM_ID;
	doomcom->numplayers = doomcom->numnodes = 1;
	doomcom->deathmatch = false;
	doomcom->consoleplayer = 0;
	return;
    }

    netgame = true;
}

/****************************************************************************/

void I_NetCmd (void)
{
    if (netdriver) netdriver->NetCmd();
}

/****************************************************************************/

void I_CleanupNetwork(void)
{
    if (netdriver) netdriver->CleanupNetwork();
}

/****************************************************************************/

