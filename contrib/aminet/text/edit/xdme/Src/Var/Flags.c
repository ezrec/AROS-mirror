/******************************************************************************

    MODUL
	flags.c

    DESCRIPTION
	lowlevel flags-support for DME/XDME

    NOTES
	is used by vars.c

    BUGS
	<none known>

    TODO
	-/-

    EXAMPLES
	-/-

    SEE ALSO
	vars.c

    INDEX

    HISTORY
	09-11-92 b_noll created
	15-08-93 b_noll updated

******************************************************************************/

/*
**  (C)Copyright 1992 by Bernd Noll for null/zero-soft
**  All Rights Reserved
**
**  RCS Header: $Id$
**
**  Basic Functions to work with (arrays of) DME-Flags
**  (both Local and Global Flags use these functions)
**
**  Put together to start a little bit of modularisation
**  in the sources.
*/

/**************************************
		Includes
**************************************/
#include "defs.h"


/**************************************
	    Globale Exports
**************************************/
Prototype int	 test_arg  (char*, int);
Prototype char * GetFlag   (char*, char*, int, char*);
Prototype char	 SetFlag   (char*, char*, char*, int, char*);
Prototype int	 IsFlagSet (char*, int, int);


/**************************************
      Interne Defines & Strukturen
**************************************/
static const UBYTE bit[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };


/*
 *  test_arg  (previous toggler)
 *
 *  simple hack do be used if you wanna implement
 *  a function which can regognize on/off/toggle for
 *  a BOOL (boolean) variable
 *  args: 1 == TRUE == set == on | 0 == FALSE == reset == off | toggle == switch
 */

#define TA_HSH(x,y) ((x & 223) | ((y & 223) << 8))

int test_arg (char * str, int bool)
{

    if (!str)
	abort (bool);

    switch (TA_HSH(str[0], str[1]))
    {
    case TA_HSH('r','e'): /* reset */
    case TA_HSH('0', 0 ): /* 0 */
    case TA_HSH('f','a'): /* false */
    case TA_HSH('o','f'): /* off */
	return (0);
    case TA_HSH('s','e'): /* set */
    case TA_HSH('1', 0 ): /* 1 */
    case TA_HSH('t','r'): /* true */
    case TA_HSH('o','n'): /* on */
	return (1);
    case TA_HSH('t','o'): /* toggle */
    case TA_HSH('s','w'): /* switch  */
	return (!bool);
    default:
	/* error ("test_arg:\n'%s' is not a valid\nvalue for flags!", str); */
	abort (bool);
    } /* switch */
} /* test_arg */



int IsFlagSet (char * toggles, int number, int max)
{
    if (number < 0 || number >= max)
    {
	/* error ("IsFlagSet:\n'%ld' is not in range\nof [0 ..%ld] !", number, 0, max-1); */
	abort(0);
    }

    return((toggles[number/8] & bit[number&7]) ? 1 : 0);
} /* IsFlagSet */





/* short : get a number for our Get-/Set-Flag */
/* -1 -> super range */
/* -2 -> no number   */
/* -3 -> wrong start */
/* >= 0 -> use it    */

static int checknum (char* name, char* identifier, int max)
{
    int num;

    num = strlen (identifier);

    /* ---- check for a valid name */

    if (strncmp (name, identifier, num))
	return (-3);

    name += num;
    if (!is_number (name))
	return (-2);

    /* ---- check for a valid range */

    num = atoi (name);
    if (num >= max || num < 0)
	return (-1);

    return (num);
} /* checknum */

/*
 * if the string in name matches the format <xy><num>
 *  <xy>  eq identifier
 *  <num> /N in [0..max]
 * then the flag <num> in toggles is set according to value
 */

char SetFlag (char * toggles, char * name, char * value, int max, char * identifier)
{
    int num;  /* total bit number -> BYTE number */
    int bn;   /* bit number in BYTE ... */
    int res;  /* set or not */

    switch (num = checknum(name, identifier, max))
    {
    case -3:		/* no header-match */
    case -2:		/* header not followed by number */
	return (0);
    case -1:		/* range error */
	/* error ("SetFlag:\nrange error!"); */
	abort (1);
    default:		/* ok */
	bn  = num & 7;
	num = num / 8;
	res = test_arg(value, toggles[num] & bit[bn]);
	toggles[num] = (toggles[num] & ~bit[bn]) | (res ? bit[bn] : 0);  /* shorter: (res * bit[bn]) */
	return(1);
    } /* switch */
} /* SetFlag */



/*
 * if the string in name matches the format <xy><num>
 *  <xy>  eq identifier
 *  <num> /N in [0..max]
 * then a string is allocated and filled according to flag <num> in toggles
 */

char * GetFlag (char * toggles, char * name, int max, char * identifier)
{
    int num;
    char* str;

    switch (num = checknum(name, identifier, max))
    {
    case -3:		/* no header-match */
    case -2:		/* header not followed by number */
	return (NULL);
    case -1:		/* range error */
	/* error ("GetFlag:\nrange error!"); */
	abort (NULL);
    default:		/* ok */
	if (!(str = malloc(2)))
	{
	    nomemory ();
	    return NULL;
	} /* if */

	str[0] = '0' + IsFlagSet(toggles, num, max);
	str[1] =  0;

	return(str);
    } /* switch */
} /* GetFlag */


/******************************************************************************
*****  ENDE flags.c
******************************************************************************/

