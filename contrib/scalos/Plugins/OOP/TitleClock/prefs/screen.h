/*
 *  screen.h - Functions for grabbing and creating public screens
 *
 * $Date$
 * $Revision$
 *
 *  0.1
 *  20010717 DM Created - only a function for finding public screens ATM
 *
 */


#ifndef SCREEN_H
#define SCREEN_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef INTUITION_SCREENS_H
#include <intuition/screens.h>
#endif

struct Screen *FindPubScreen(STRPTR name);

#endif /* SCREEN_H */


