/****************************************************************

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

 Includes and other common stuff for the MUI demo programs

***************************************************************/

/* MUI */
#include <libraries/mui.h>

/* System */
#include <dos/dos.h>
#include <graphics/gfxmacros.h>
#include <workbench/workbench.h>

/* Prototypes */
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/utility.h>
#include <proto/asl.h>

#include <proto/muimaster.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//extern struct Library *SysBase,*IntuitionBase,*UtilityBase,*GfxBase,*DOSBase,*IconBase;
struct Library *MUIMasterBase = NULL;
#if defined(__amigaos4__)
struct MUIMasterIFace * IMUIMaster = NULL;
#endif // __amigaos4__

/*************************/
/* Init & Fail Functions */
/*************************/

static VOID fail(APTR app,char *str)
{
        if (app)
                MUI_DisposeObject(app);

#ifndef _DCC
        #if defined(__amigaos4__)
        if (IMUIMaster)
            DropInterface( (struct Interface*)IMUIMaster );
        #endif //__amigaos4__
        if (MUIMasterBase)
                CloseLibrary(MUIMasterBase);
#endif

        if (str)
        {
                puts(str);
                exit(20);
        }
        exit(0);
}


#ifdef _DCC

int brkfunc(void) { return(0); }

int wbmain(struct WBStartup *wb_startup)
{
        extern int main(int argc, char *argv[]);
        return (main(0, (char **)wb_startup));
}

#endif


#ifdef __SASC
int CXBRK(void) { return(0); }
int _CXBRK(void) { return(0); }
void chkabort(void) {}
#endif


static VOID init(VOID)
{
#ifdef _DCC
        onbreak(brkfunc);
#endif

#ifndef _DCC
        if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN)))
                fail(NULL,"Failed to open "MUIMASTER_NAME".");

        #if defined(__amigaos4__)
        if (!(IMUIMaster = (struct MUIMasterIFace*)GetInterface( MUIMasterBase, "main", 1, 0)))
                fail(NULL,"Failed to GetInterface "MUIMASTER_NAME".");
        #endif // __amigaos4__
#endif
}


#ifndef __SASC
static VOID stccpy(char *dest,char *source,int len)
{
        strncpy(dest,source,len);
        dest[len-1]='\0';
}
#endif


#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif


LONG __stack = 8192;
