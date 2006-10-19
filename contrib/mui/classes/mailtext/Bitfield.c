/***************************************************************************

 Mailtext - An MUI Custom Class for eMessage display
 Copyright (C) 1996-2001 by Olaf Peters
 Copyright (C) 2002-2006 by Mailtext Open Source Team

 GCC & OS4 Portage by Alexandre Balaban

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Mailtext OpenSource project :  http://sourceforge.net/projects/mailtext/

 $Id$

***************************************************************************/

#include "Bitfield.h"
#include <string.h>

/*/// "BOOL BF_IsSet(Bitfield *bitfield, unsigned char bit)" */

/****** Bitfield/BF_IsSet ****************************************************

    NAME
        BF_IsSet -- check, if a bit is set

    SYNOPSIS
        isset = BF_IsSet(Bitfield *bitfield, unsigned char bit)

        BOOL BF_IsSet(Bitfield *, unsigned char)

    FUNCTION
        checks whether the specified bit is set in the bitfield.

    INPUTS
        bitfield -- pointer to a Bitfield structure. The structure will not
                      be modified.
        bit -- number of the bit to be checked.

    RESULT
        isset -- TRUE, if the bit is currently set in the Bitfield, FALSE
                   otherwise.

    SEE ALSO
        Bitfield/BF_Set, Bitfield/BF_Unset

******************************************************************************
*
*/

BOOL BF_IsSet(const Bitfield *bitfield, const unsigned char bit)
{
    return (BOOL) (bitfield->bits[(int)bit>>3] & (BYTEBITS)(1 << (bit & 7))) ;
}

/*\\\*/

/*/// "void BF_Set(Bitfield *bitfield,   unsigned char bit)" */

/****** Bitfield/BF_Set ******************************************************

    NAME
        BF_Set -- set a bit

    SYNOPSIS
        BF_Set(Bitfield *bitfield, unsigned char bit)

        void BF_Set(Bitfield *, unsigned char)

    FUNCTION
        Sets the specified bit in the bitfield.

    INPUTS
        bitfield -- pointer to a Bitfield structure.
        bit -- number of the bit to be set.

    SEE ALSO
        Bitfield/BF_IsSet, Bitfield/BF_Unset

******************************************************************************
*
*/

void BF_Set(Bitfield *bitfield,   const unsigned char bit)
{
    int i = (int)(bit>>3) ;
    bitfield->bits[i] |= (BYTEBITS)(1 << (bit & 7)) ;
}

/*\\\*/

/*/// "void BF_Unset(Bitfield *bitfield, unsigned char bit)" */

/****** Bitfield/BF_Unset ****************************************************

    NAME
        BF_Unset -- clear a bit

    SYNOPSIS
        BF_Unset(Bitfield *bitfield, unsigned char bit)

        void BF_Unset(Bitfield *, unsigned char)

    FUNCTION
        Clears the specified bit in the bitfield.

    INPUTS
        bitfield -- pointer to a Bitfield structure.
        bit -- number of the bit to be cleared.

    SEE ALSO
        Bitfield/BF_Set, Bitfield/BF_IsSet

******************************************************************************
*
*/

void BF_Unset(Bitfield *bitfield, const unsigned char bit)
{
    int i = (int)(bit>>3) ;

    bitfield->bits[i] &= (BYTEBITS)((BYTEBITS)0xff - (BYTEBITS)(1 << (bit & 7))) ;
}

/*\\\*/

/*/// "void BF_ClearBitfield(Bitfield *bitfield)" */

/****** Bitfield/BF_ClearBitfield ********************************************

    NAME
        BF_ClearBitfield -- clear all bits in a Bitfield.

    SYNOPSIS
        BF_ClearBitfieldt(Bitfield *bitfield)

        void BF_ClearBitfield(Bitfield *)

    FUNCTION
        All bits in the Bitfield will be cleared, i.e. IsSet will return
        FALSE for all bits.

        This function *must* be used before any operation on the bitfield,
        otherwise the operation may return unexpected results.

    INPUTS
        bitfield -- pointer to a Bitfield structure.

    SEE ALSO
        Bitfield/BF_IsSet, Bitfield/BF_Unset

******************************************************************************
*
*/
void BF_ClearBitfield(Bitfield *bitfield)
{
    int i ;

    for (i = 0; i < 32; i++)
    {
        bitfield->bits[i] = 0 ;
    }
}

/*\\\*/

/*/// "void BF_FillBitfield(Bitfield *bitfield, unsigned char *bits)" */

/****** Bitfield/BF_FillBitfield *********************************************

    NAME
        BF_FillBitfield -- set multiple bits in one call.

    SYNOPSIS
        BF_FillBitfield(Bitfield *bitfield, unsigned char *bits)

        void BF_FillBitfield(Bitfield *, unsigned char *)

    FUNCTION
        Sets all bits in the bits array in the given bitfield, so BF_IsSet
        will return TRUE for every single bit specified in the bits array.

    INPUTS
        bitfield -- pointer to a Bitfield structure.
        bits -- array with the bits to be set.

    BUGS
        As the bits-array is handled as a string, the bit 0 (NULL) will
        *not* be set in the bitfield. If you need to set the bit 0, you
        have to set it explicitly with a BF_Set(bitfield, 0) call.

    SEE ALSO
        Bitfield/BF_Set, Bitfield/BF_IsSet

******************************************************************************
*
*/

void BF_FillBitfield(Bitfield *bitfield, const unsigned char *bits)
{
    int i, l = strlen(bits) ;

    for (i = 0; i < l; i++)
    {
        BF_Set(bitfield, (unsigned char)bits[i]) ;
    }
}


/*\\\*/

