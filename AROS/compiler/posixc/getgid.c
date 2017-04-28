/*
    Copyright � 2003-2013, The AROS Development Team. All rights reserved.
    $Id$

    POSIX.1-2008 function getgid().
*/

/*****************************************************************************

    NAME */
#include <unistd.h>

	gid_t getgid(

/*  SYNOPSIS */
	void)

/*  FUNCTION
	Returns the group ID of the calling process

    INPUTS
	
    RESULT
	
    NOTES
        Always return 0 for the moment.

    EXAMPLE

    BUGS
    	
    SEE ALSO
    	setgid()
        
    INTERNALS

******************************************************************************/
{
    return (gid_t)0;
} /* getgid() */
