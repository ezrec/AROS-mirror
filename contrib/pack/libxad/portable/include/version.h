#ifndef XADMASTER_VERSION_C
#define XADMASTER_VERSION_C

/*  $Id$
    the xad unarchiving library system version data

    XAD library system for archive handling
    Copyright (C) 1998 and later by Dirk Stöcker <soft@dstoecker.de>

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
*/

#define BETA
#define XADMASTERVERSION   13
#define XADMASTERREVISION  1
#define DATETXT            "27.06.2011"

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION            "13.1"
#endif

#define LIBNAME   "xadmaster.library"

#ifdef DEBUG
  #define ADDTXTDEB  " DEBUG"
#else
  #define ADDTXTDEB  ""
#endif

#ifdef BETA
  #define ADDTXTBETA " BETA"
#else
  #define ADDTXTBETA ""
#endif

#ifdef DEBUGRESOURCE
  #define ADDTXTRES " RESOURCETRACK"
#else
  #define ADDTXTRES ""
#endif

#ifdef __MORPHOS__
  #define ADDTXTCPU     " MorphOS"
#elif defined(_M68060)
  #define ADDTXTCPU     " 060"
#elif defined(_M68040)
  #define ADDTXTCPU     " 040"
#elif defined(_M68030)
  #define ADDTXTCPU     " 030"
#elif defined(_M68020)
  #define ADDTXTCPU     " 020"
#else
  #define ADDTXTCPU     ""
#endif

#ifdef XAD_GPLCLIENTS
  #define DISTRIBUTION " (GPL)"
#else
  #define DISTRIBUTION " (LGPL)"
#endif

#define IDSTRING "xadmaster " VERSION " (" DATETXT ")" ADDTXTDEB ADDTXTCPU ADDTXTBETA ADDTXTRES \
                 DISTRIBUTION " by Dirk Stöcker <soft@dstoecker.de>\r\n"

#endif /* XADMASTER_VERSION_C */
