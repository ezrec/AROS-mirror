/* $Id$ */

/*
     AHI - Hardware independent audio subsystem
     Copyright (C) 1996-1999 Martin Blom <martin@blom.org>
     
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
     
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
     
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the
     Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
     MA 02139, USA.
*/

#ifndef _PPCHEADER_H_
#define _PPCHEADER_H_

#include <dos/dosextens.h>

#include "ahi_def.h"

struct PowerPCContext
{
  volatile int              Command;
  volatile void*            Argument;

  struct Process*           SlaveProcess;
  struct Process*           MainProcess;

  void*                     CurrentMixBuffer;

  int                       Active;
  struct Hook*              Hook;
  void*	                    Dst;
  void*                     XLock;

  struct AHIPrivAudioCtrl*  AudioCtrl;
  struct Library*           PowerPCBase;
};

struct PPCHandlerMessage
{
  struct Message           Message;
  struct AHIPrivAudioCtrl* AudioCtrl;
};

#define PPCC_COM_NONE          0
#define PPCC_COM_START         1
#define PPCC_COM_ACK           2
#define PPCC_COM_INIT          3
#define PPCC_COM_SOUNDFUNC     4
#define PPCC_COM_QUIT          5
#define PPCC_COM_DEBUG         6
#define PPCC_COM_FINISHED      7


#endif /* _PPCHEADER_H_ */
