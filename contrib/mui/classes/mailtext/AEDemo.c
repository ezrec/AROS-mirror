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

 Example for AttributesEngine

***************************************************************************/

#include <exec/memory.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <exec/exec.h>
#include <utility/tagitem.h>
#include <stdio.h>
#include <string.h>

#include "AttributesEngine.h"

#define MUID_Mailtext_DisablePattern         "(begin [0-9][0-9][0-9] #?|-----BEGIN PGP (SIGNATURE|MESSAGE|PUBLIC KEY BLOCK)-----)"
#define MUID_Mailtext_EnablePattern          "(end|-----END PGP (SIGNATURE|MESSAGE|PUBLIC KEY BLOCK)-----)"
#define MUID_Mailtext_NonIntroducingChars    " :.,^+\\|?([{&%$§\"!"
#define MUID_Mailtext_QuoteChars             ">"
#define MUID_Mailtext_URLChars               "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$-_.+!*'(),;/?:@&=%~"
#define MUID_Mailtext_TerminatingChars       " )]}.,:+-'&\"!?*/_#"
#define MUID_Mailtext_WhitespaceChars        " ([{\"'&+*/_#"

/*/// "void SetupBitfield(Bitfield *bf, char *bits)" */

void SetupBitfield(Bitfield *bf, char *bits)
{
    BF_ClearBitfield(bf) ;
    if (bits)
    {
        BF_FillBitfield(bf, bits) ;
    }
}

/*\\\*/

/*/// "void SetupParams(struct AEParams *p)" */

void SetupParams(struct AEParams *p)
{

    INIT(p->dpattern) ;
    INIT(p->epattern) ;

    p->showAttrs    = FALSE ;
    p->underlineURL = TRUE ;
    p->separateSig  = FALSE ;

    DupString(&p->dpattern, MUID_Mailtext_DisablePattern) ;
    DupString(&p->epattern, MUID_Mailtext_EnablePattern) ;

    SetupBitfield(&p->scharsBF,  MUID_Mailtext_NonIntroducingChars) ;
    SetupBitfield(&p->tcharsBF,  MUID_Mailtext_TerminatingChars) ;
    SetupBitfield(&p->wscharsBF, MUID_Mailtext_WhitespaceChars) ;
    SetupBitfield(&p->attsBF,    "*/_#") ;
    SetupBitfield(&p->qcharsBF,  MUID_Mailtext_QuoteChars) ;
    SetupBitfield(&p->urlBF,     MUID_Mailtext_URLChars) ;

    BF_FillBitfield(&p->wscharsBF, MUID_Mailtext_QuoteChars) ;

    strcpy(p->qchars, MUID_Mailtext_QuoteChars) ;

    p->sigSeparator[0] = '\0' ;

    strcpy(p->bdata.set,   "\033[1m") ;
    strcpy(p->bdata.unset, "\033[22m") ;
    p->bdata.setSize   = 4 ;
    p->bdata.unsetSize = 5 ;
    p->bdata.active    = FALSE ;
    p->bdata.dist      = 80 ;

    strcpy(p->idata.set,   "\033[3m") ;
    strcpy(p->idata.unset, "\033[23m") ;
    p->idata.setSize   = 4 ;
    p->idata.unsetSize = 5 ;
    p->idata.active    = FALSE ;
    p->idata.dist      = 80 ;

    strcpy(p->udata.set,   "\033[4m") ;
    strcpy(p->udata.unset, "\033[24m") ;
    p->udata.setSize   = 4 ;
    p->udata.unsetSize = 5 ;
    p->udata.active    = FALSE ;
    p->udata.dist      = 80 ;

    strcpy(p->tcolor.set,   "\033[33m") ;
    strcpy(p->tcolor.unset, "\033[0m") ;
    p->tcolor.setSize   = 5 ;
    p->tcolor.unsetSize = 4 ;
    p->tcolor.active    = FALSE ;
    p->tcolor.dist      = 80 ;

    strcpy(p->ucolor.set,   "\033[34m") ;
    strcpy(p->ucolor.unset, "\033[0m") ;
    p->ucolor.setSize   = 5 ;
    p->ucolor.unsetSize = 4 ;
    p->ucolor.active    = FALSE ;

    strcpy(p->qcolors[0].set,   "\033[32m") ;
    strcpy(p->qcolors[0].unset, "\033[0m") ;
    p->qcolors[0].setSize   = 5 ;
    p->qcolors[0].unsetSize = 4 ;

    strcpy(p->qcolors[1].set,   "\033[33m") ;
    strcpy(p->qcolors[1].unset, "\033[0m") ;
    p->qcolors[1].setSize   = 5 ;
    p->qcolors[1].unsetSize = 4 ;

    strcpy(p->qcolors[2].set,   "\033[32m") ;
    strcpy(p->qcolors[2].unset, "\033[0m") ;
    p->qcolors[2].setSize   = 5 ;
    p->qcolors[2].unsetSize = 4 ;

    strcpy(p->qcolors[3].set,   "\033[33m") ;
    strcpy(p->qcolors[3].unset, "\033[0m") ;
    p->qcolors[3].setSize   = 5 ;
    p->qcolors[3].unsetSize = 4 ;

    strcpy(p->qcolors[4].set,   "\033[32m") ;
    strcpy(p->qcolors[4].unset, "\033[0m") ;
    p->qcolors[4].setSize   = 5 ;
    p->qcolors[4].unsetSize = 4 ;

    p->qdist = 8 ;

}

/*\\\*/

int main(int argc, char *argv[])
{
    struct RDArgs *rdArgs ;
    struct FileInfoBlock *fib ;
    BPTR fh ;
    char *text, *ntext ;
    LONG args[2] = {0L, 0L};
    LONG size = 0 ;

    ntext = 0 ;
    text = 0 ;

    if (rdArgs = ReadArgs("FILE/A", args, 0))
    {
        if (args[0])
        {
            if (fh = Open((STRPTR)args[0], MODE_OLDFILE))
            {
                if (fib = AllocDosObjectTags(DOS_FIB, TAG_DONE))
                {
                    if (ExamineFH(fh, fib))
                    {
                        if (text = AllocVec(fib->fib_Size+1, MEMF_PUBLIC | MEMF_CLEAR))
                        {
                            if (Read(fh, text, fib->fib_Size) == fib->fib_Size)
                            {
                                text[fib->fib_Size] = '\0' ;
                                size = fib->fib_Size ;
                            }
                            else
                            {
                                (void)PrintFault(IoErr(), "AEDemo") ;
                                FreeVec(text) ;
                            }
                        }
                    }
                    FreeDosObject(DOS_FIB, fib) ;
                }
                (void)Close(fh) ;
            }
            else
            {
                (void)PrintFault(IoErr(), "AEDemo") ;
            }
        }
        FreeArgs(rdArgs) ;
    }
    else
    {
        (void)PrintFault(IoErr(), "AEDemo") ;
    }

    if (text)
    {
        ntext = AllocVec(size+4096, MEMF_PUBLIC | MEMF_CLEAR) ;

        if (ntext)
        {
            struct AEParams p ;

            SetupParams(&p) ;

            AE_SetAttributes(text, ntext, size+4096, &p) ;

            printf("%s\033[0m\n", ntext) ;

            FreeVec(ntext) ;
        }

        FreeVec(text) ;
    }
}



