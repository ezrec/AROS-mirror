
#include "class.h"

/****************************************************************************/

char ASM
getKeyChar(REG(a0) STRPTR string)
{
    register char res = 0;

    if (string)
    {
        register char *c;

        for (c = string; *c && *c!='_'; c++);
        if (*c++) res = ToLower(*c);
    }

    return res;
}

/***********************************************************************/

Object * ASM
keyCheckmark(REG(a0) STRPTR key,REG(d0) ULONG help)
{
    register Object *obj;

    if ((obj = MUI_MakeObject(MUIO_Checkmark,key)))
        SetAttrs(obj,MUIA_CycleChain,1,
                     MUIA_ShortHelp,getString(help),
                     TAG_DONE);

    return obj;
}

/****************************************************************************/

Object * ASM
keyPopPen(REG(d0) ULONG title,REG(d1) STRPTR key,REG(d2) ULONG help)
{
    return PoppenObject,
        MUIA_FixWidthTxt,  "nn",
        MUIA_FixHeightTxt, "nn",
        MUIA_ControlChar,  getKeyChar(key),
        MUIA_ShortHelp,    getString(help),
        MUIA_CycleChain,   TRUE,
        MUIA_Draggable,    TRUE,
        MUIA_Window_Title, getString(title),
    End;
}

/****************************************************************************/

Object * ASM
urltext(REG(a0) STRPTR url,REG(a1) STRPTR text)
{
    return UrltextObject,
        MUIA_Urltext_Url,   url,
        MUIA_Urltext_Text,  text,
    End;
}

/****************************************************************************/

#ifndef __AROS__
struct stream
{
    char    *buf;
    int     size;
    int     counter;
    int     stop;
};

static void ASM
___stuff(REG(d0) char c,REG(a3) struct stream *s)
{
    if (!s->stop)
    {
        if (++s->counter>=s->size)
        {
            *(s->buf) = 0;
            s->stop   = 1;
        }
        else *(s->buf++) = c;
    }
}

int STDARGS
snprintf(char *buf,int size,char *fmt,...)
{
    struct stream s;

    s.buf     = buf;
    s.size    = size;
    s.counter = 0;
    s.stop    = 0;

    RawDoFmt(fmt,&fmt+1,(APTR)___stuff,&s);

    return s.counter-1;
}
#endif /* !__AROS__ */

/****************************************************************************/
