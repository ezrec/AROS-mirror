/*
    Copyright © 2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#define MUIMASTER_YES_INLINE_STDARG

#include <proto/muimaster.h>
#include <proto/intuition.h>

#include "support.h"

Object *MakeButton(CONST_STRPTR label, CONST_STRPTR shortHelp, ULONG inputMode)
{
    return TextObject,
        ButtonFrame,
        MUIA_Background,           MUII_ButtonBack,
        MUIA_Text_PreParse, (IPTR) "\ec",
        MUIA_Text_Contents, (IPTR) label,
        MUIA_ShortHelp,     (IPTR) shortHelp,
        MUIA_InputMode,            inputMode,
    End;
}

Object *MakeToggleButton(CONST_STRPTR label, CONST_STRPTR shortHelp)
{
    return MakeButton(label, shortHelp, MUIV_InputMode_Toggle);
}

Object *MakePushButton(CONST_STRPTR label, CONST_STRPTR shortHelp)
{
    return MakeButton(label, shortHelp, MUIV_InputMode_RelVerify);
}
