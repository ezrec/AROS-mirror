
#include "class.h"

/***********************************************************************/

struct stream
{
    char *buf;
    int  size;
    int  counter;
    int  stop;
};

static void ASM
putChar(REG(d0) char c,REG(a3) struct stream *s)
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

    RawDoFmt(fmt,&fmt+1,(APTR)putChar,&s);

    return s.counter-1;
}

/****************************************************************************/
