#ifndef PROTO_CYBERGRAPHICS_H
#define PROTO_CYBERGRAPHICS_H

/*
**	$Id$
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	(C) Copyright 2003-2004 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif
#ifndef GRAPHICS_RASTPORT_H
#include <graphics/rastport.h>
#endif
#ifndef GRAPHICS_VIEW_H
#include <graphics/view.h>
#endif
#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef UTILITY_HOOKS_H
#include <utility/hooks.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
extern struct Library * CyberGfxBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/cybergraphics.h>
 #ifdef __USE_INLINE__
  #include <inline4/cybergraphics.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_CYBERGRAPHICS_PROTOS_H
  #define CLIB_CYBERGRAPHICS_PROTOS_H 1
 #endif /* CLIB_CYBERGRAPHICS_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct CyberGfxIFace *ICyberGfx;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_CYBERGRAPHICS_PROTOS_H
  #include <clib/cybergraphics_protos.h>
 #endif /* CLIB_CYBERGRAPHICS_PROTOS_H */
 #if defined(__GNUC__)
  #ifdef __AROS__
   #include <defines/cybergraphics.h>
  #else
   #ifndef __PPC__
    #include <inline/cybergraphics.h>
   #else
    #include <ppcinline/cybergraphics.h>
   #endif /* __PPC__ */
  #endif /* __AROS__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/cybergraphics_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/cybergraphics_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_CYBERGRAPHICS_H */
