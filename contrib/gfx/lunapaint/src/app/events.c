/****************************************************************************
*                                                                           *
* events.h -- Lunapaint,                                                    *
*    http://developer.berlios.de/projects/lunapaintami/                     *
* Copyright (C) 2006, 2007, Hogne Titlestad <hogga@sub-ether.org>           *
* Copyright (C) 2009-2011 LunaPaint Development Team                        *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program; if not, write to the Free Software Foundation,   *
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.            *
*                                                                           *
****************************************************************************/
#include  <SDI_hook.h>

#include "events.h"

struct IOStdReq *gameInput;
struct MsgPort *gameInputPort;
struct Interrupt *gameInputInterrupt;

struct Hook DisableKeyboard_hook;
struct Hook EnableKeyboard_hook;

BOOL evalMouseButtonL;
BOOL evalMouseButtonR;
LONG evalMouseX;
LONG evalMouseY;
ULONG evalRawKey;
ULONG releaseKey;

HOOKPROTONHNO(DisableKeyboard_func, void, int *param)
{
    keyboardEnabled = FALSE;

}

HOOKPROTONHNO(EnableKeyboard_func, void, int *param)
{

    keyboardEnabled = TRUE;

}


HOOKPROTO(handleEvents, struct InputEvent *, APTR obj, APTR param) 
{
   struct InputEvent *event = (struct InputEvent *)hook; 

    if ( event )
    {
        switch ( event->ie_Code )
        {
            case IECODE_LBUTTON:
                evalMouseButtonL = TRUE;
                break;
            case IECODE_LBUTTON | IECODE_UP_PREFIX:
                evalMouseButtonL = FALSE;
                break;
            case IECODE_RBUTTON:
                evalMouseButtonR = TRUE;
                break;
            case IECODE_RBUTTON | IECODE_UP_PREFIX:
                evalMouseButtonR = FALSE;
                break;
            default:
                break;
        }
        if ( event->ie_Class == IECLASS_RAWKEY )
            evalRawKey = event->ie_Code;
    }

    return event;
}

BOOL InitEvents ( )
{
    // Ready hooks
    EnableKeyboard_hook.h_Entry = ( HOOKFUNC )EnableKeyboard_func;
    DisableKeyboard_hook.h_Entry = ( HOOKFUNC )DisableKeyboard_func;

    /* Setup all parts */
    gameInputPort = CreateMsgPort ( );
    if ( !gameInputPort ) return FALSE;

    gameInput = ( struct IOStdReq *)CreateIORequest ( gameInputPort, sizeof ( struct IOStdReq ) );
    if ( !gameInput ) return FALSE;

    BYTE inputsuccess = OpenDevice ( "input.device", 0, ( struct IORequest *)gameInput, 0 );
    if ( inputsuccess != 0 )
    {
        D(bug( "Failed to open input device, %d.\n", ( LONG )gameInput->io_Error ));
        return FALSE;
    }

    gameInputInterrupt = AllocVec ( sizeof ( struct Interrupt ), MEMF_PUBLIC|MEMF_CLEAR );
    if ( !gameInputInterrupt )
    {
        ShutdownEvents ( );
        return FALSE;
    }

    /* Add event handler */
    gameInputInterrupt->is_Code = ( APTR )&handleEvents;
    gameInputInterrupt->is_Data = NULL;
    gameInputInterrupt->is_Node.ln_Pri = 100;
    gameInputInterrupt->is_Node.ln_Name = "easyinput handler";
    gameInput->io_Command = IND_ADDHANDLER;
    gameInput->io_Data = ( APTR )gameInputInterrupt;

    if ( DoIO ( ( struct IORequest *)gameInput ) )
    {
        ShutdownEvents ( );
        return FALSE;
    }

    return TRUE;
}

void ShutdownEvents ( )
{
    if ( !gameInput ) return;

    /* Remove handler */
    gameInput->io_Command = IND_REMHANDLER;
    gameInput->io_Data = ( APTR )gameInputInterrupt;
    DoIO ( ( struct IORequest *)gameInput );

    /* Close device and clean up */
    if ( gameInput->io_Error == 0 )
        CloseDevice ( ( struct IORequest *)gameInput );
    DeleteIORequest ( ( struct IORequest *)gameInput );
    DeleteMsgPort ( gameInputPort );
    FreeVec ( gameInputInterrupt );
}

IPTR CheckEvent ( ULONG event )
{
    switch ( event )
    {
        case eventMouseButtonLeft:
            return evalMouseButtonL;
            break;
        case eventMouseButtonRight:
            return evalMouseButtonR;
        default:
            return ( IPTR )FALSE;
    }
    return 0;
}
