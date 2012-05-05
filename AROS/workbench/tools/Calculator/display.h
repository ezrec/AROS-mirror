/*
    Copyright (C) 2012, The AROS Development Team.
    $Id$
*/

#include <libraries/mui.h>

#define CALCDISPFLAG_HEXMODE    (1 << 0)

#define CALCDISPOP_ADD    1
#define CALCDISPOP_SUB    2
#define CALCDISPOP_MUL    3
#define CALCDISPOP_DIV    4

#define MUIA_CalcDisplay_Input 0x80088008

struct CalcDisplay_DATA
{
    char *disp_buff;
    char *disp_prev;
    ULONG displ_operator;
    ULONG displ_flags;
};
