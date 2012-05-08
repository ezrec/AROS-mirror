/*
    Copyright (C) 2012, The AROS Development Team.
    $Id$
*/

#include <libraries/mui.h>

#define CALCDISPFLAG_HEXMODE (1 << 0)
#define CALCDISPFLAG_HASPERIOD (1 << 5)
#define CALCDISPFLAG_CLEAROP (1 << 30)
#define CALCDISPFLAG_CALCULATED (1 << 31)

#define CALCDISPOP_NONE   0
#define CALCDISPOP_ADD    1
#define CALCDISPOP_SUB    2
#define CALCDISPOP_MUL    3
#define CALCDISPOP_DIV    4

#define MUIA_CalcDisplay_Input          0x80088008
#define MUIA_CalcDisplay_Calculated     0x80088009
#define MUIA_CalcDisplay_Base          0x8008800A

#define MUIM_CalcDisplay_DoCurrentStep 0x80088008

#define MUIV_CalcDisplay_MaxInputLen 32

struct CalcDisplay_DATA
{
    char *disp_buff;
    char *disp_prev;
    struct TextFont   *disp_font;
    struct TextAttr   disp_textattr;
    ULONG displ_operator;
    ULONG displ_flags;
};
