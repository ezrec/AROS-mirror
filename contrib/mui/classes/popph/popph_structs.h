
/*
** $Id: popph_structs.h,v 1.5 2000/04/04 17:58:19 carlos Exp $
**
** © 1999 Marcin Orlowski <carlos@amiga.com.pl>
*/

#include "popplaceholder_mcc.h"

// Popph_data
struct Data
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

