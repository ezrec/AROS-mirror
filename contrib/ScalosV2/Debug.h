#ifndef  DEBUG_H
#define  DEBUG_H

/*
**      $VER: debug_protos.h
**
**      C prototypes. For use with 32 bit integers only.
**
**  done by Stefan Sommerfeld of ALiENDESiGN!
**
**      (C) Copyright 1990-1993 Commodore-Amiga, Inc.
**          All Rights Reserved
*/

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/

LONG KCmpStr(char *string1, char *string2);
LONG KGetchar(void);
LONG KGetNum(void);
LONG KMayGetchar(void);
void KPrintF(char *format, ...);
void KVPrintF(char *format, APTR data);
LONG KPutchar(LONG ch);
void KPutStr(char *string);

/* ------ PPC stubs -------- */
void KPrintFPPC(char *format, ...);

#ifdef DEBUG_SERIAL

#ifdef __STORM__

#define DEBUG(x) KPrintF(__FILE__" "__FUNC__" (Line: %ld): "x, __LINE__);
#define DEBUG1(x,y) KPrintF(__FILE__" "__FUNC__" (Line: %ld): "x, __LINE__, y);
#define DEBUG2(x,y,z) KPrintF(__FILE__" "__FUNC__" (Line: %ld): "x, __LINE__, y, z);
#define DEBUG3(x,y,z,a) KPrintF(__FILE__" "__FUNC__" (Line: %ld): "x, __LINE__, y, z, a);
#define DEBUG4(x,y,z,a,b) KPrintF(__FILE__" "__FUNC__" (Line: %ld): "x, __LINE__, y, z, a, b);

#else /* __STORM__ */

#define DEBUG(x) KPrintF(__FILE__" (Line: %ld): "x, __LINE__);
#define DEBUG1(x,y) KPrintF(__FILE__" (Line: %ld): "x, __LINE__, y);
#define DEBUG2(x,y,z) KPrintF(__FILE__" (Line: %ld): "x, __LINE__, y, z);
#define DEBUG3(x,y,z,a) KPrintF(__FILE__" (Line: %ld): "x, __LINE__, y, z, a);
#define DEBUG4(x,y,z,a,b) KPrintF(__FILE__" (Line: %ld): "x, __LINE__, y, z, a, b);

#endif /* __STORM__ */

#else /* DEBUG_SERIAL */

#define DEBUG(x) /* NO DEBUG! */
#define DEBUG1(x,y) /* NO DEBUG! */
#define DEBUG2(x,y,z) /* NO DEBUG! */
#define DEBUG3(x,y,z,a) /* NO DEBUG! */
#define DEBUG4(x,y,z,a,b) /* NO DEBUG! */

#endif /* DEBUG_SERIAL */

#endif /* DEBUG_H */
