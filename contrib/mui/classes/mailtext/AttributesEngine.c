/***************************************************************************

 Mailtext - An MUI Custom Class for eMessage display
 Copyright (C) 1996-2001 by Olaf Peters
 Copyright (C) 2002-2006 by Mailtext Open Source Team

 GCC & OS4 Portage by Alexandre Balaban

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Mailtext OpenSource project :  http://sourceforge.net/projects/mailtext/

 $Id$

 Implementation for the attribute engine

***************************************************************************/

#include "AttributesEngine.h"
#include "URLs.h"

#include <string.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include <exec/memory.h>

#include <libraries/mui.h>

#define MUISTUFF

struct AttrData
{
    BOOL    active ;
    STRPTR  set,
            unset ;
    LONG    dist,
            setSize,
            unsetSize ;
} ;

/*/// "static LONG GetLine(char *line, char *src, const LONG srcPos)" */

static LONG GetLine(char *line, char *src, const LONG srcPos)
{
    LONG cnt = 0 ;

    if (!src[srcPos])
    {
        return (-1) ;
    }

    src += srcPos ;

    while ((*src) && (*src != '\n') && (*src != '\r') && (cnt < AE_LINESIZE))
    {
        *line++ = *src++ ;
        cnt++ ;
    }

    *line = (char)0 ;

    return (cnt) ;
}

/*\\\*/
/*/// "static STRPTR PreparePatternBuffer(void *pool, const STRPTR pattern)" */

static STRPTR PreparePatternBuffer(void *pool, const STRPTR pattern)
{
    LONG size = 0;
    void *res ;

    if( 0 != pattern ) size = strlen(pattern);

    if (!size)
    {
        return (0) ;
    }

    size = (size * 2) + 2 ;

    if (!(res = (void*)AllocPooled(pool, size)))
    {
        return (0) ;
    }

    if (ParsePatternNoCase(pattern, res, size) == -1)
    {
        FreePooled(pool, res, size) ;
        return (0) ;
    }

    return (res) ;
}

/*\\\*/
/*/// "static BOOL QuoteLine(const char *line, const char *qchars, const LONG qdist)" */

static BOOL QuoteLine(const char *line, const char *qchars, const LONG qdist)
{
    LONG index ;

    if ((index = (LONG)strcspn(line, qchars)) < qdist)
    {
        if (strcspn(line, "<[(") < index)
        {
            return (FALSE) ;
        }
        else
        {
            return (BOOL)line[index] ;
        }
    }
    else
    {
        return (FALSE) ;
    }
}

/*\\\*/
/*/// "static LONG Concat(char *dst, char *src, LONG pos, const LONG dstsize)" */

static LONG Concat(char *dst, char *src, LONG pos, const LONG dstsize)
{
    while ((pos < dstsize-1) && (*src))
    {
        dst[pos++] = *src++ ;
    }
    dst[pos] = '\0' ;

    return (pos) ;
}

/*\\\*/
/*/// "static LONG ConcatChar(char *dst, const char c, LONG pos, const LONG dstsize)" */

static LONG ConcatChar(char *dst, const char c, LONG pos, const LONG dstsize)
{
    if (pos < dstsize-1)
    {
        dst[pos++] = c ;
    }
    dst[pos] = '\0' ;

    return (pos) ;
}

/*\\\*/
/*/// "static LONG DistanceToNextLegal(const char *line, const char attribute, const Bitfield *tchars, const Bitfield *wschars, const Bitfield *atts)" */

static LONG DistanceToNextLegal(char *line, const char attribute, const Bitfield *tchars, const Bitfield *wschars, const Bitfield *atts)
{
    char *pos = line ;

    if (*pos)
    {
        pos = strchr(pos, attribute) ;
        if (!pos)
        {
            return (LONG)0 ;
        }

        // if the char in front of the attribute is a whitespace and not
        // another attributechar, we fail.

        if (!BF_IsSet(atts, *(pos-1)) && BF_IsSet(wschars, *(pos-1)))
        {
            return (LONG)0 ;
        }

        pos++ ;

        if ((!(*pos)) || (BF_IsSet(tchars, *pos) && ((*pos) != attribute)))
        {
            return (LONG)pos-(LONG)line-1 ;
        }
        else
        {
            return (LONG) 0 ;
        }
    }
    else
    {
        return (LONG)0 ;
    }
}

/*\\\*/
/*/// "static LONG URLCheck(const char *src, const LONG *pos, const Bitfield *urlchars, const Bitfield *tchars, const char *set, const char *unset, const char *dst, const LONG dpos, const LONG dstsize, const struct AEParams *p)" */

static LONG URLCheck(char *src, LONG *pos, const Bitfield *urlchars, const Bitfield *tchars, char *set, char *unset, char *dst, LONG dpos, const LONG dstsize, struct AEParams *p)
{
    BOOL isURL = IsURL(&src[*pos], urlchars) ;

    if (!isURL)
    {
        return (dpos) ;
    }

    dpos = Concat(dst, set, dpos, dstsize) ;

    if (p->underlineURL)
    {
        dpos = Concat(dst, p->udata.set, dpos, dstsize) ;
    }

    while (BF_IsSet(urlchars, src[*pos]) && (BF_IsSet(urlchars, src[(*pos)+1]) || (src[*pos] == '/') || (!BF_IsSet(tchars, src[*pos]))))
    {
        dpos = ConcatChar(dst, src[(*pos)++], dpos, dstsize) ;
    }

    if (p->underlineURL)
    {
        dpos = Concat(dst, p->udata.unset, dpos, dstsize) ;
    }

    dpos = Concat(dst, unset, dpos, dstsize) ;

    return (dpos) ;
}

/*\\\*/
/*/// "static BOOL CheckForAttribute(const char *sline, LONG fspos, const Bitfield *wschars, const Bitfield *schars, const Bitfield *atts)" */

static BOOL CheckForAttribute(const char *sline, LONG fspos, const Bitfield *wschars, const Bitfield *schars, const Bitfield *atts)
{
    BOOL res = ((!fspos) || (BF_IsSet(wschars, sline[fspos-1]) && (sline[fspos-1] != sline[fspos]))) ;

    if (!res)
    {
        return (res) ;
    }
    else
    {
        while (sline[fspos+1] && BF_IsSet(atts, sline[fspos+1]))
        {
            fspos++ ;
        }
        return (BOOL) (res && sline[fspos+1] && !BF_IsSet(schars, sline[fspos+1])) ;
    }
}

/*\\\*/

/*/// "BOOL AE_SetAttributes(const STRPTR src, STRPTR dst, long dstsize, struct AEParams *p)" */

BOOL AE_SetAttributes(const STRPTR src, STRPTR dst, long dstsize, struct AEParams *p)
{
    STRPTR sline ;
    LONG srcsize, spos = 0, dpos = 0, llen = 0, quotelevel ;
    BOOL quoteCheck, res, parsingActive ;

    STRPTR dpattern, epattern, spattern ;
#ifdef MUISTUFF
    STRPTR sbpattern ;
#endif

    void *pool ;

    if (!(src && dst && (dstsize>0)))
    {
        return (FALSE) ;
    }

    srcsize = strlen(src) ;

    --dstsize ;

    if ((!srcsize) || (srcsize > dstsize))
    {
        return (FALSE) ;
    }

    res = FALSE ;
    parsingActive = TRUE ;

    if (pool = (void *)CreatePool(MEMF_CLEAR || MEMF_PUBLIC, 32768, 32768))
    {
        quoteCheck = ( (p->qdist) && (p->qchars[0]) ) ;

        dpattern = PreparePatternBuffer(pool, STR(p->dpattern)) ;
        epattern = PreparePatternBuffer(pool, STR(p->epattern)) ;
        spattern = PreparePatternBuffer(pool, STR(p->spattern)) ;
#ifdef MUISTUFF
        sbpattern = PreparePatternBuffer(pool, "(<tsb>|<sb>)#?") ;
#endif

        sline = (void*)AllocPooled(pool, AE_LINESIZE) ;

        if (sline)
        {
            while (llen >= 0)
            {
                llen = GetLine(sline, src, spos) ;

                if (llen == -1)
                {
                    res = TRUE ;
                    break ;
                }

                spos += llen ;

                if (src[spos] == '\r')
                {
                    spos++ ;
                }

                if (src[spos] == '\n')
                {
                    spos++ ;
                }

                if (parsingActive)
                {
                    if (dpattern)
                    {
                        parsingActive = !MatchPatternNoCase(dpattern, sline) ;
                    }
                }
                else
                {
                    if (epattern)
                    {
                        parsingActive = MatchPatternNoCase(epattern, sline) ;
                    }
                }

                if (sline[0])
                {
                    if (!parsingActive)
                    {
                        dpos = Concat(dst, sline, dpos, dstsize) ;
                    }
                    else
                    {
                        if (p->separateSig && MatchPatternNoCase(spattern, sline))
                        {
                            dpos = Concat(dst, p->sigSeparator, dpos, dstsize) ;
                            // dpos = Concat(dst, sline, dpos, dstsize) ;
                        }
                        else
                        {

                            LONG fspos = 0 ;
#ifdef MUISTUFF
                            BOOL center = FALSE ;
#endif
                            struct AEAttrData *data = NULL;

                            quotelevel = 0 ;

#ifdef MUISTUFF
                            if (MatchPatternNoCase(sbpattern, sline))
                            {
                                if (sline[3] == '>')
                                {
                                    dpos = Concat(dst, "\033E", dpos, dstsize) ;
                                    fspos = 4 ;
                                }
                                else
                                {
                                    dpos = Concat(dst, "\033E\033t", dpos, dstsize) ;
                                    fspos = 5 ;
                                }
                                if (sline[fspos])
                                {
                                    dpos = Concat(dst, "\033c", dpos, dstsize) ;
                                    center = TRUE ;

                                    if (sline[fspos] == ' ')
                                    {
                                        fspos++ ;
                                    }
                                }

                            }
                            else
#endif
                            if (quoteCheck && QuoteLine(sline, p->qchars, p->qdist))
                            {
                                BOOL done = FALSE ;
                                quotelevel = 1 ;

                                while (!done)
                                {
                                    dpos = Concat(dst, p->qcolors[quotelevel-1].set, dpos, dstsize) ;

                                    do
                                    {
                                        if (dpos < dstsize-1)
                                        {
                                            dst[dpos++] = sline[fspos] ;
                                        }

                                    } while (!BF_IsSet(&p->qcharsBF, sline[fspos++])) ;

                                    dst[dpos] = '\0' ;

                                    if (!(done = !QuoteLine(&sline[fspos], p->qchars, p->qdist)))
                                    {
                                        done = (++quotelevel > 5) ;
                                    }
                                }
                            }

                            while (sline[fspos])
                            {
                                if ((!(p->bdata.active || p->idata.active || p->udata.active)) && strchr("FfHhGgMmNnTtWwPp", sline[fspos]) && ((!fspos) || (strchr(":\t", sline[fspos-1])) || BF_IsSet(&p->qcharsBF, sline[fspos-1]) || BF_IsSet(&p->wscharsBF, sline[fspos-1])))
                                {
                                    if (p->tcolor.active)
                                    {
                                        dpos = URLCheck(sline, &fspos, &p->urlBF, &p->tcharsBF, (char *)&p->ucolor.set, (char *)&p->tcolor.set, dst, dpos, dstsize, p) ;
                                    }
                                    else if (quotelevel > 0)
                                    {
                                        dpos = URLCheck(sline, &fspos, &p->urlBF, &p->tcharsBF, (char *)&p->ucolor.set, (char *)p->qcolors[quotelevel-1].set, dst, dpos, dstsize, p) ;
                                    }
                                    else
                                    {
                                        dpos = URLCheck(sline, &fspos, &p->urlBF, &p->tcharsBF, (char *)&p->ucolor.set, (char *)&p->ucolor.unset, dst, dpos, dstsize, p) ;
                                    }

                                    // check if the URL reached the end of the line:

                                    if (!sline[fspos])
                                    {
                                        continue ;
                                    }
                                }

                                if (BF_IsSet(&p->attsBF, sline[fspos]))
                                {
                                    switch (sline[fspos])
                                    {

                                        case '*' :
                                            data = &p->bdata ;
                                            break ;

                                        case '/' :
                                            data = &p->idata ;
                                            break ;

                                        case '#' :
                                            data = &p->tcolor ;
                                            break ;

                                        case '_' :
                                            data = &p->udata ;
                                            break ;

                                    } /* switch */

                                    if (data->active)
                                    {
                                        if ((!sline[fspos+1]) || BF_IsSet(&p->tcharsBF, sline[fspos+1]))
                                        {
                                            dpos = Concat(dst, data->unset, dpos, dstsize) ;
                                            data->active = FALSE ;

                                            if ((quotelevel > 0) && (data == &p->tcolor))
                                            {
                                                dpos = Concat(dst, p->qcolors[quotelevel-1].set, dpos, dstsize) ;
                                            }

                                            if (p->bdata.active)
                                            {
                                                dpos = Concat(dst, p->bdata.set, dpos, dstsize) ;
                                            }

                                            if (p->idata.active)
                                            {
                                                dpos = Concat(dst, p->idata.set, dpos, dstsize) ;
                                            }

                                            if (p->udata.active)
                                            {
                                                dpos = Concat(dst, p->udata.set, dpos, dstsize) ;
                                            }

                                            if (p->showAttrs)
                                            {
                                                dpos = ConcatChar(dst, sline[fspos], dpos, dstsize) ;
                                            }
                                        }
                                        else
                                        {
                                            dpos = ConcatChar(dst, sline[fspos], dpos, dstsize) ;
                                        }
                                    }
                                    else
                                    {
                                        BF_Unset(&p->attsBF, sline[fspos]) ;

                                        if ( CheckForAttribute(sline, fspos, &p->wscharsBF, &p->scharsBF, &p->attsBF) )
                                        {
                                            LONG dist = DistanceToNextLegal(&sline[fspos+1], sline[fspos], &p->tcharsBF, &p->wscharsBF, &p->attsBF) ;

                                            if (dist && (dist <= (LONG)data->dist))
                                            {
                                                if (p->showAttrs)
                                                {
                                                    dpos = ConcatChar(dst, sline[fspos], dpos, dstsize) ;
                                                }
                                                dpos = Concat(dst, data->set, dpos, dstsize) ;
                                                data->active = TRUE ;
                                            }
                                            else
                                            {
                                                dpos = ConcatChar(dst, sline[fspos], dpos, dstsize) ;
                                            }
                                        }
                                        else
                                        {
                                            dpos = ConcatChar(dst, sline[fspos], dpos, dstsize) ;
                                        }

                                        BF_Set(&p->attsBF, sline[fspos]) ;
                                    }
                                }
                                else
                                {
                                    dpos = ConcatChar(dst, sline[fspos], dpos, dstsize) ;
                                }

                                fspos++ ;
                            } /* while */

#ifdef MUISTUFF
                            if (center)
                            {
                                dpos = Concat(dst, "\033n", dpos, dstsize) ;
                            }
#endif
                        }
                    }

                }

                dpos = ConcatChar(dst, '\n', dpos, dstsize) ;

                if (dpos >= dstsize-1)
                {
                    break ;
                }
            } /* while */
        }

        DeletePool(pool) ;
    }

    if (!res)
    {
        strcpy(dst, src) ;
    }

    return (TRUE) ;
}

/*\\\*/

