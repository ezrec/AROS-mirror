/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#include "system_headers.h"
#include "Scout_rev.h"

CONST_STRPTR USED_VAR vers = VERS;
CONST_STRPTR USED_VAR vstring = VERS " " CPU " (" DATE ")";
CONST_STRPTR USED_VAR verstag = VERSTAG;
CONST_STRPTR USED_VAR version_date = PROGNAME " " VERS " (" DATE ")";
CONST_STRPTR USED_VAR release = "Release " RELEASEVERSION "." RELEASEREVISION " (" COMPILER ")";
CONST ULONG version_ulong = VERSION;
CONST ULONG revision_ulong = REVISION;

