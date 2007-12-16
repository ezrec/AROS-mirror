
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
#endif

/****************************************************************************/
