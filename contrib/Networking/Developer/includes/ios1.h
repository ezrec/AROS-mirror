#ifndef IOS1_H
#define IOS1_H
/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

*/

/*
 * Include SAS/C ios1.h
 */
#ifndef _IOS1_H
#include <include:ios1.h>
#endif

/*
 * Additional flag definitions for the ufbflg field
 * The given value was recommended by SAS.
 */
#ifndef UFB_SOCK
#define UFB_SOCK 0x100 /* AmiTCP/IP socket */
#endif

/*
 * Additional external definitions
 */
extern struct UFB *__allocufb(int *);
#define   allocufb    __allocufb

#endif
