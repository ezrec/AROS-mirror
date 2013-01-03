/*
    Copyright © 2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "aros/i386/asm.h"

                    .balign 32,0x90
                    .globl  Exec_Wbinvd
                    .type   Exec_Wbinvd,@function

Exec_Wbinvd:
                    wbinvd
                    iret
