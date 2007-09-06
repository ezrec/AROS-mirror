#ifndef POPPLACEHOLDER_PRIVATE_H
#define POPPLACEHOLDER_PRIVATE_H

/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <utility/date.h>

/*** Instance data **********************************************************/
struct Popplaceholder_Data
{
    struct Hook DisplayHook;
    struct Hook WindowHook;
    struct Hook ObjStrHook;

    APTR   asl_req;
    long   asl_type;
    long   asl_active;

    struct MUI_CustomClass *CL_String;
    Object *str;
    Object *lv;
    Object *popbutton;
    Object *popaslbutton;
    Object *popobj;

    long   separator;
    long   copyentries;
    long   replacemode;
    long   singlecolumn;
    long   autoclose;

    char   title1[ POPPH_MAX_KEY_LEN ];
    char   title2[ POPPH_MAX_STRING_LEN ];
};


struct PPHS_Data
{
    Object *dropobj;

    ULONG  tag_contents;
    ULONG  tag_bufpos;
    ULONG  tag_maxlen;

    ULONG  separator;
    ULONG  replacemode;
    ULONG  stringtype;
    ULONG  autoclose;

    APTR   contextmenu;
    ULONG  cmenu_enabled;

    ULONG  clip_device;
};

#endif /* POPPLACEHOLDER_PRIVATE_H */

