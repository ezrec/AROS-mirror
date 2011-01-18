/*
 *  REXX.H
 *
 *	(c) Copyright 1988 Kim DeVaughn, All Rights Reserved
 *
 *  Definitions for the ARexx interface, in addition to the ARexx includes.
 *
 */

#ifndef REXX_STORAGE_H
#include <rexx/storage.h>
#endif

#ifndef REXX_H
#define REXX_H



/*  More RexxMsg field definitions, to make life a little easier.	     */

#define  ACTION(rmp)     (rmp->rm_Action)    /* command (action) code        */
#define  RESULT1(rmp)    (rmp->rm_Result1)   /* primary result (return code) */
#define  RESULT2(rmp)    (rmp->rm_Result2)   /* secondary result             */
#define  COMM_ADDR(rmp)  (rmp->rm_CommAddr)  /* host address (port name)     */
#define  FILE_EXT(rmp)   (rmp->rm_FileExt)   /* file extension               */



/*
 *  Error Level Definitions   [ PRELIMINARY - SUBJECT TO CHANGE ]
 *
 *  Definitions for internal  (dme)  error reporting begin with CMD_
 *  Definitions for external (ARexx) error reporting begin with MAC_
 *
 *
 *	     0 - command execution successful [normal]	   (i.e., title = OK)
 *	     1 - command execution successful [alternate]  (i.e., title = OK)
 *	     2 - command execution successful [normal]	   (don't alter title)
 *	     3 - command execution successful [alternate]  (don't alter title)
 *	     4 - reserved
 *	     5 - command could not complete requested function
 *	    10 - command was unable to process correctly
 *	    20 - command failure detected - data integrity in question
 *	    50 - internal error detected by  dme  or  ARexx
 *
 *  Only levels 0 and 1 allow the ARexx interface code to explicitly set title.
 *
 *
 *  Eg:      0 - the normal case ("OK")
 *	     1 - a secondary succeeful result ("OK")
 *	     2 - normal, but may have put up an informational message
 *	     3 - secondary normal, but may have informational message
 *	     4 -
 *	     5 - "find" didn't find, "file not found", "block not marked", etc.
 *	    10 - "command not found", "syntax error", etc.
 *	    15 - a sever failure; "write failed", "unable to open write file", etc.
 *	    20 - maybe a sequence of commands that failed; may have modified data
 *	    50 - dunno, but definitely bad shit
 */

#define  CMD_VALID0	    0
#define  CMD_VALID1	    1
#define  CMD_VALID2	    2
#define  CMD_VALID3	    3
#define  CMD_VALID4	    4
#define  CMD_FAILED	    5
#define  CMD_ERROR	   10
#define  CMD_SEVERE	   15
#define  CMD_ABORT	   20
#define  CMD_MALFUNCTION   50


/*  Similar to the internal command error levels above, for the moment. */

#define  MAC_VALID0	    0
#define  MAC_VALID1	    1
#define  MAC_VALID2	    2
#define  MAC_VALID3	    3
#define  MAC_VALID4	    4
#define  MAC_FAILED	    5
#define  MAC_ERROR	   10
#define  MAC_SEVERE	   15
#define  MAC_ABORT	   20
#define  MAC_MALFUNCTION   50


#define  CMD_INITIAL	   0
#define  TITLE_THRESHHOLD  CMD_VALID1

#endif /* REXX_H */

