#ifndef PROTO_IDENTIFY_H
#define PROTO_IDENTIFY_H

/*
**	$Id$
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	(C) Copyright 2003-2005 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * IdentifyBase;
 #else
  extern struct Library * IdentifyBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/identify.h>
 #ifdef __USE_INLINE__
  #include <inline4/identify.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_IDENTIFY_PROTOS_H
  #define CLIB_IDENTIFY_PROTOS_H 1
 #endif /* CLIB_IDENTIFY_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct IdentifyIFace *IIdentify;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_IDENTIFY_PROTOS_H
  #include <clib/identify_protos.h>
 #endif /* CLIB_IDENTIFY_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/identify.h>
  #else
   #include <ppcinline/identify.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/identify_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/identify_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_IDENTIFY_H */
