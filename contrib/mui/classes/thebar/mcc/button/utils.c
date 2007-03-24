/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2007 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id: utils.c 49 2007-02-27 23:38:13Z damato $

***************************************************************************/

#include "class.h"

#include "SDI_stdarg.h"
#include "mcc_common.h"

#include "Debug.h"

#include <stdlib.h>

#include <proto/input.h>

/***********************************************************************/

#ifndef __MORPHOS__
// DoSuperNew()
// Calls parent NEW method within a subclass
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  Object *rc;
  VA_LIST args;

  VA_START(args, obj);
  rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, VA_ARG(args, ULONG), NULL);
  VA_END(args);

  return rc;
}
#endif

/***********************************************************************/

void stripUnderscore(STRPTR dest, STRPTR from, ULONG mode)
{
  BOOL done = FALSE;
  int i = TB_MAXLABELLEN;

  ENTER();

  while(*from != '\0' && --i)
  {
    if(*from == '_')
    {
      if(done)
      {
        if (mode == STRIP_First)
          *dest++ = *from;
      }
      else
        done = TRUE;
    }
    else
      *dest++ = *from;

    from++;
  }

  *dest = '\0';

  LEAVE();
}

/***********************************************************************/

#ifndef __MORPHOS__

static int stcd_l(const char *in, long *value)
{
  if(in)
  {
    char *ptr;

    switch(*in)
    {
      case '+':
      case '-':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        *value = strtol(in, &ptr, 10);
        return ptr - in;
      break;
    }
  }

  *value = 0;

  return 0;
}

#endif

/***********************************************************************/

struct TextFont *openFont(STRPTR name)
{
  char buf[256];
  STRPTR t, s;
  struct TextAttr ta;
  long ys;
  struct TextFont *font;

  ENTER();

  strlcpy(buf, name, sizeof(buf));

  SHOWSTRING(DBF_GUI, buf);

  if((t = strchr(buf,'/')) != NULL)
  {
    *t++ = 0;
    if(!stcd_l(t,&ys) || ys<=0)
      ys = 8;
  }
  else
    ys = 8;

  for(s = NULL, t = buf; *t; t++)
    if(*t == '.')
      s = t;
  if(s == NULL || stricmp(++s, "font"))
    strlcat(buf, ".font", sizeof(buf));

  SHOWSTRING(DBF_GUI, buf);

  memset(&ta, 0, sizeof(ta));
  ta.ta_Name  = buf;
  ta.ta_YSize = ys;
  ta.ta_Style = 0;
  ta.ta_Flags = 0;

  font = OpenDiskFont(&ta);

  RETURN(font);
  return font;
}

/***********************************************************************/

APTR allocVecPooled(APTR pool, ULONG size)
{
  ULONG *mem;

  ENTER();

  size += sizeof(ULONG);

  if((mem = AllocPooled(pool, size)) != NULL)
      *mem++ = size;

  RETURN(mem);
  return mem;
}

/****************************************************************************/

void freeVecPooled(APTR pool, APTR mem)
{
  ENTER();

  FreePooled(pool, (LONG *)mem-1, *((LONG *)mem-1));

  LEAVE();
}

/****************************************************************************/

APTR allocArbitratePooled(ULONG size)
{
  APTR mem;

  ENTER();

  ObtainSemaphore(&lib_poolSem);
  mem = AllocPooled(lib_pool,size);
  ReleaseSemaphore(&lib_poolSem);

  RETURN(mem);
  return mem;
}

/***********************************************************************/

void freeArbitratePooled(APTR mem, ULONG size)
{
  ENTER();

  ObtainSemaphore(&lib_poolSem);
  FreePooled(lib_pool,mem,size);
  ReleaseSemaphore(&lib_poolSem);

  LEAVE();
}

/***********************************************************************/

APTR allocArbitrateVecPooled(ULONG size)
{
  APTR mem;

  ENTER();

  ObtainSemaphore(&lib_poolSem);
  mem = allocVecPooled(lib_pool,size);
  ReleaseSemaphore(&lib_poolSem);

  RETURN(mem);
  return mem;
}

/***********************************************************************/

void freeArbitrateVecPooled(APTR mem)
{
  ENTER();

  ObtainSemaphore(&lib_poolSem);
  freeVecPooled(lib_pool,mem);
  ReleaseSemaphore(&lib_poolSem);

  LEAVE();
}

/***********************************************************************/

ULONG peekQualifier(void)
{
  ULONG rc = 0;
  struct MsgPort *port;

  ENTER();

  if((port = CreateMsgPort()) != NULL)
  {
    struct IORequest *iorequest;

    if((iorequest = CreateIORequest(port, sizeof(*iorequest))) != NULL)
    {
      if(OpenDevice("input.device", 0, iorequest, 0) == 0)
      {
        struct Library *InputBase = (struct Library *)iorequest->io_Device;
        #ifdef __amigaos4__
        struct InputIFace *IInput;
        GETINTERFACE(IInput, InputBase);
        #endif
        rc = PeekQualifier();

        DROPINTERFACE(IInput);
        CloseDevice(iorequest);
      }
      DeleteIORequest(iorequest);
    }
    DeleteMsgPort(port);
  }

  RETURN(rc);
  return(rc);
}

/***********************************************************************/

