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
#ifndef SCALOS_REV_H
#include "scalos_rev.h"
#define SCALOS_REV_H
#endif /* SCALOS_REV_H */

// set CPU define automagically according to SAS/C compiler flag
#ifdef _M68060
        #define CPU "68060"
#else
        #ifdef _M68040
                #define CPU "68040"
        #else
                #ifdef _M68030
                        #define CPU "68030"
                #else
                        #ifdef _M68000
                                #define CPU "68000"
                        #else
                                #ifdef __PPC__
                                        #define CPU "PPC"
                                #endif
                        #endif
                #endif
        #endif
#endif

#define VERSION_STRING VERS" "CPU
