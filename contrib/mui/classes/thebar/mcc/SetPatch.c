/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

#if defined(__amigaos3__)
#include <proto/exec.h>
#include <exec/types.h>
#include <exec/lists.h>
#include <exec/semaphores.h>

ULONG setPatchVersion;

void GetSetPatchVersion(void)
{
  struct SetPatchSemaphore
  {
    struct SignalSemaphore semaphore;
    struct MinList private;
    UWORD version;
    UWORD revision;
  } *sem;

  Forbid();
  if((sem = (struct SetPatchSemaphore *)FindSemaphore((STRPTR)"« SetPatch »")) != NULL)
  {
    setPatchVersion = ((ULONG)sem->version << 16) | sem->revision;
  }
  Permit();
}
#endif
