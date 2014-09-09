/***************************************************************************

  muibuilder.library - Support library for MUIBuilder's code generators
  Copyright (C) 1990-2009 by Eric Totel
  Copyright (C) 2010-2011 by MUIBuilder Open Source Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  MUIBuilder Support Site: http://sourceforge.net/projects/muibuilder/

  $Id$

***************************************************************************/

#ifndef BASE_H
#define BASE_H

#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <dos/bptr.h>

struct LibraryHeader
{
  struct Library          libBase;
  struct Library          *sysBase;
  BPTR                    segList;
  struct SignalSemaphore  libSem;
};

/***************************************************************************/

extern struct LibraryHeader* MUIBBase;

#if defined(__amigaos4__)
#define __BASE_OR_IFACE_TYPE	struct MUIBIFace *
#define __BASE_OR_IFACE_VAR		IMUIB
#else
#define __BASE_OR_IFACE_TYPE	struct LibraryHeader *
#define __BASE_OR_IFACE_VAR		MUIBBase
#endif
#define __BASE_OR_IFACE			__BASE_OR_IFACE_TYPE __BASE_OR_IFACE_VAR

/***************************************************************************/

#endif
