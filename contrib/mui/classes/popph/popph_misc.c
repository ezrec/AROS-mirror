/*
** $Id$
**
** © 1999 Marcin Orlowski <carlos@amiga.com.pl>
*/


#include "popph_private.h"

// _strlen
int _strlen( char *str, char stop )
{
    int i=0;

    while(str[i] != stop)
	i++;

    return i;
}

// _strchr
char *_strchr( char *buf, char key )
{
    char *found = NULL;
    int  i;

    for( i = 0; ; i++ )
    {
	if( buf[i] == 0 )
	    break;

	if( buf[i] == key )
	{
	    found = &buf[i];
	    break;
	}
    }

    return found;
}
