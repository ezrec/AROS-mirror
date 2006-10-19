/***************************************************************************

 Mailtext - An MUI Custom Class for eMessage display
 Copyright (C) 1996-2001 by Olaf Peters
 Copyright (C) 2002-2006 by Mailtext Open Source Team

 Based upon code written by Matthias Scheler <matthias@scheler.de>

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

 Mailtext header file for includes

***************************************************************************/

#include <clib/alib_protos.h>
#include <clib/alib_stdio_protos.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/rdargs.h>
#include <exec/memory.h>
#include <exec/types.h>
#include <libraries/asl.h>
#include <libraries/locale.h>
#include <libraries/mui.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <pragmas/muimaster_pragmas.h>
#include <string.h>
#include <utility/tagitem.h>

#include "AttributesEngine.h"
#include "Bitfield.h"
#include "DynStr.h"
#include "Mailtext_private.h"
#include "loc/Mailtext_mcp.h"
#include "loc/Mailtext_mcc.h"
#include "URLs.h"
