/*
     emu10kx.audio - AHI driver for SoundBlaster Live! series
     Copyright (C) 2002-2003 Martin Blom <martin@blom.org>

     This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
     as published by the Free Software Foundation; either version 2
     of the License, or (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <config.h>

#include <clib/alib_protos.h>
#include <proto/utility.h>

#include "library.h"
#include "8010.h"
#include "hwaccess.h"

/******************************************************************************
** Read from AC97 register ****************************************************
******************************************************************************/

ULONG
AC97GetFunc( struct Hook*           hook,
	     struct EMU10kxBase*    EMU10kxBase,
	     struct AC97GetMessage* msg )
{
  struct DriverBase*  AHIsubBase = (struct DriverBase*) EMU10kxBase;
  struct EMU10kxData* dd;

  if( msg->CardNum >= (ULONG) EMU10kxBase->cards_found ||
      EMU10kxBase->driverdatas[ msg->CardNum ] == NULL )
  {
    Req( "No valid EMU10kxData for AC97 card %ld.", msg->CardNum );
    return ~0UL;
  }

  dd = EMU10kxBase->driverdatas[ msg->CardNum ];

  return emu10k1_readac97( &dd->card, msg->Register );
}


/******************************************************************************
** Write to AC97 register *****************************************************
******************************************************************************/

VOID
AC97SetFunc( struct Hook*           hook,
	     struct EMU10kxBase*    EMU10kxBase,
	     struct AC97SetMessage* msg )
{
  struct DriverBase*  AHIsubBase = (struct DriverBase*) EMU10kxBase;
  struct EMU10kxData* dd;

  if( msg->CardNum >= (ULONG) EMU10kxBase->cards_found ||
      EMU10kxBase->driverdatas[ msg->CardNum ] == NULL )
  {
    Req( "No valid EMU10kxData for AC97 card %ld.", msg->CardNum );
    return;
  }

  dd = EMU10kxBase->driverdatas[ msg->CardNum ];

  emu10k1_writeac97( &dd->card, msg->Register, msg->Value );
}
