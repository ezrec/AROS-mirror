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

#ifndef _ADDROUTINES_H_
#define _ADDROUTINES_H_

#include <config.h>
#include <CompilerSpecific.h>
#include "ahi_def.h"

/*
** Samples          Number of samples to calculate.
** ScaleLeft        Left volume multiplier.
** ScaleRight       Right volume multiplier (not used for mono sounds).
** StartPointLeft   Sample value from last session, for interpolation. Update!
** StartPointRight  Sample value from last session, for interpolation. Update!
** Src              Pointer to source samples.
** Dst              Pointer to pointer to destination buffer. Update!
** FirstOffsetI     The offset value of the first sample (when StartPoint* 
**                  should be used).
** Offset           The offset (fix-point). Update!
** Add              Add value (fix-point).
** StopAtZero       If true, abort at next zero-crossing.
*/

#define ADDARGS LONG      Samples,\
                LONG      ScaleLeft,\
                LONG      ScaleRight,\
                LONG	 *StartPointLeft,\
                LONG	 *StartPointRight,\
                void     *Src,\
                void    **Dst,\
                LONG	  FirstOffsetI,\
                Fixed64   Add,\
                Fixed64  *Offset,\
                BOOL      StopAtZero

typedef LONG (ADDFUNC)(ADDARGS);

LONG AddByteMono( ADDARGS );
LONG AddByteStereo( ADDARGS );
LONG AddBytesMono( ADDARGS );
LONG AddBytesStereo( ADDARGS );
LONG AddWordMono( ADDARGS );
LONG AddWordStereo( ADDARGS );
LONG AddWordsMono( ADDARGS );
LONG AddWordsStereo( ADDARGS );

LONG AddByteMonoB( ADDARGS );
LONG AddByteStereoB( ADDARGS );
LONG AddBytesMonoB( ADDARGS );
LONG AddBytesStereoB( ADDARGS );
LONG AddWordMonoB( ADDARGS );
LONG AddWordStereoB( ADDARGS );
LONG AddWordsMonoB( ADDARGS );
LONG AddWordsStereoB( ADDARGS );

LONG AddLofiByteMono( ADDARGS );
LONG AddLofiByteStereo( ADDARGS );
LONG AddLofiBytesMono( ADDARGS );
LONG AddLofiBytesStereo( ADDARGS );
LONG AddLofiWordMono( ADDARGS );
LONG AddLofiWordStereo( ADDARGS );
LONG AddLofiWordsMono( ADDARGS );
LONG AddLofiWordsStereo( ADDARGS );

LONG AddLofiByteMonoB( ADDARGS );
LONG AddLofiByteStereoB( ADDARGS );
LONG AddLofiBytesMonoB( ADDARGS );
LONG AddLofiBytesStereoB( ADDARGS );
LONG AddLofiWordMonoB( ADDARGS );
LONG AddLofiWordStereoB( ADDARGS );
LONG AddLofiWordsMonoB( ADDARGS );
LONG AddLofiWordsStereoB( ADDARGS );

#endif /* _ADDROUTINES_H_ */
