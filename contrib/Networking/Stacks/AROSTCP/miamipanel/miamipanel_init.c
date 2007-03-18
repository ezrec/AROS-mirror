/*
    Copyright © 2002-2006, The AROS Development Team. 
    All rights reserved.
    
    $Id: miamipanel_init.c 24652 2006-08-21 10:35:08Z verhaegs $
*/

#include <exec/types.h>
#include <exec/libraries.h>
#include <aros/libcall.h>
#include <aros/symbolsets.h>

#include <proto/intuition.h>
#include <proto/cybergraphics.h>

#include "miamipanel_intern.h"
#include LC_LIBDEFS_FILE

#include <aros/debug.h>

/****************************************************************************************/

static int Init(LIBBASETYPEPTR LIBBASE)
{
    D(bug("Inside Init func of MiamiPanel.library\n"));
    
    return TRUE;
}

/****************************************************************************************/

static int Expunge(LIBBASETYPEPTR LIBBASE)
{
    D(bug("Inside Expunge func of MiamiPanel.library\n"));
    
    return TRUE;
}

ADD2INITLIB(Init, 0);
ADD2EXPUNGELIB(Expunge, 0);
