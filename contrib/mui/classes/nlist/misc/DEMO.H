/*************************************************************/
/* Includes and other common stuff for the MUI demo programs */
/*************************************************************/

/*
#include <proto/alib.h>
#include <inline/muimaster.h>
#include <libraries/mui.h>

#include <inline/exec.h>
#include <libraries/mui.h>
#include <exec/memory.h>
*/




/* MUI */
#include <libraries/mui.h>
#include <clib/muimaster_protos.h>

/* System */
#include <dos/dos.h>
#include <graphics/gfxmacros.h>
#include <workbench/workbench.h>

/* Prototypes */
/*
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/utility_protos.h>
#include <clib/asl_protos.h>
*/

#include <inline/muimaster.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Library *MUIMasterBase;


/*************************/
/* Init & Fail Functions */
/*************************/

static VOID fail(APTR app,char *str)
{
  if (app)
    MUI_DisposeObject(app);

  if (MUIMasterBase)
    CloseLibrary(MUIMasterBase);

  if (str)
  {
    puts(str);
    exit(20);
  }
  exit(0);
}



static VOID init(VOID)
{
  if (!(MUIMasterBase = (struct Library *) OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN)))
    fail(NULL,"Failed to open "MUIMASTER_NAME".");
}


static VOID stccpy(char *dest,char *source,int len)
{
        strncpy(dest,source,len);
        dest[len-1]='\0';
}


#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif


LONG __stack = 20000;
