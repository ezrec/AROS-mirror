/*
    Copyright � 2002, The AROS Development Team. All rights reserved.
    $Id$

    Desc: C function stccpy()
    Lang: english
*/

/*****************************************************************************

    NAME */
#include <string.h>

	size_t stccpy (

/*  SYNOPSIS */
	char	   * dest,
	const char * src,
	size_t	     n)

/*  FUNCTION
	Copy a string. Works like an assignment "dest=src;". At most
	n characters are copied.

    INPUTS
	dest - The string is copied into this variable. Make sure it is
		large enough.
	src - This is the new contents of dest.
	n - How many characters to copy at most. If the string src is
		smaller than that, only strlen(str)+1 bytes are copied.

    RESULT
	The number of copied characters.

    NOTES
	No check is beeing made that dest is large enough for src.

    EXAMPLE

    BUGS

    SEE ALSO
	strncpy()

    INTERNALS

    HISTORY

******************************************************************************/
{
    char * ptr = dest;

    while (n && *src)
    {
        *ptr = *src;
	ptr ++;
	src ++;
	n--;
    }

    *ptr = '\0';

    return (ptr-dest);
} /* stccpy */
