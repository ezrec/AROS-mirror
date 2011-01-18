
#include <exec/types.h>

/***********************************************************************/

#define DUMP     0
#define RUN      1
#define MINRUN   3
#define MAXRUN 128
#define MAXDAT 128

#define PutByte(c)    {*destination++ = (c); *putSize += 1;}
#define UPutByte(c)   (*dest++ = (c))
#define OutDump(nn)   dest = BRCPutDump(dest,nn)
#define OutRun(nn,cc) dest = BRCPutRun(dest,nn,cc)

/***********************************************************************/

static char
*BRCPutDump(char *destination,int number,char *buf,ULONG *putSize)
{
    int i;

    PutByte(number-1);
    for(i = 0; i<number; i++) PutByte(buf[i]);

    return destination;
}

/***********************************************************************/

static char
*BRCPutRun(char *destination,int number,int count,ULONG *putSize)
{
    PutByte(-(number-1));
    PutByte(count);

    return destination;
}

/***********************************************************************/

ULONG
BRCPack(char *pSource,char *pDest,LONG rowSize)
{
    char   buf[256], *source, *dest, c, lastc;
    ULONG  putSize;
    USHORT mode = DUMP;
    SHORT  nbuf, rstart;

    source = pSource;
    dest   = pDest;

    if (!source) return NULL;

    rstart  = 0;
    putSize = 0;
    buf[0]  = lastc = c = *source++;
    nbuf    = 1;
    rowSize--;

    for( ; rowSize; --rowSize)
    {
        buf[nbuf++] = c = *source++;

        switch(mode)
        {
            case DUMP:
                if (nbuf>MAXDAT)
                {
                    dest = BRCPutDump(dest,nbuf-1,buf,&putSize);
                    buf[0] = c;
                    nbuf   = 1;
                    rstart = 0;

                    break;
                }

                if (c==lastc)
                {
                    if (nbuf-rstart>=MINRUN)
                    {
                        if (rstart>0) dest = BRCPutDump(dest,rstart,buf,&putSize);
                        mode = RUN;
                    }
                    else
                        if (rstart==0)
                        {
                            mode = RUN;
                        }
                }
                else
                {
                    rstart = nbuf-1;
                }

                break;

            case RUN:
                if ((c!=lastc) || (nbuf-rstart>MAXRUN))
                {
                    dest = BRCPutRun(dest,nbuf-1-rstart,lastc,&putSize);
                    buf[0] = c;
                    nbuf   = 1;
                    rstart = 0;
                    mode   = DUMP;
                }
                break;
        }

        lastc = c;
    }

    switch(mode)
    {
        case DUMP:
            BRCPutDump(dest,nbuf,buf,&putSize);
            break;

        case RUN:
            BRCPutRun(dest,nbuf-rstart,lastc,&putSize);
            break;
    }

    return putSize;
}

/***********************************************************************/
