// Video.c
// $Date$
// $Revision$

#include <exec/types.h>
#include <scalos/scalos.h>
#include <utility/hooks.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

#include <clib/alib_protos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>

#include <defs.h>

#include "Video.h"

//---------------------------------------------------------------------------------------

BOOL InitExtra(void)
{
	return TRUE;
}

void ExitExtra(void)
{
}

BOOL InitInstance(void)
{
	return TRUE;
}

void ExitInstance(void)
{
}

//---------------------------------------------------------------------------------------

/*
The memalign() function returns a block of memory of
size bytes aligned to blocksize. The blocksize must be given
as a power of two. It sets errno and returns a null pointer upon failure.

Pointers returned by memalign() may be passed to free().
Pointers passed to realloc() are checked and if not
aligned to the system, the realloc() fails and returns NULL.
*/
void *memalign(size_t blocksize, size_t bytes)
{
	d1(KPrintF("%s/%s/%ld: blocksize=%lu  bytes=%lu\n", __FILE__, __FUNC__, __LINE__, blocksize, bytes));
	(void) blocksize;

	return malloc(bytes);
}

//-----------------------------------------------------------------------------

void *gethostbyname(const char *s)
{
	(void)s;

	return NULL;
}

//-----------------------------------------------------------------------------

/*
    These functions shall round their argument to the nearest integer value,
    rounding according to the current rounding direction.

RETURN VALUE
    Upon successful completion, these functions shall return the rounded integer value.
    [MX] [Option Start] If x is NaN, a domain error shall occur, and an unspecified value is returned.
    If x is +Inf, a domain error shall occur and an unspecified value is returned.
    If x is -Inf, a domain error shall occur and an unspecified value is returned.
    If the correct value is positive and too large to represent as a long long, a domain error shall occur and an unspecified value is returned.
    If the correct value is negative and too large to represent as a long long, a domain error shall occur and an unspecified value is returned. [Option End]
*/

long long int llrint(double x)
{
#warning need some work here
	return (long long) x;
}

//-----------------------------------------------------------------------------

