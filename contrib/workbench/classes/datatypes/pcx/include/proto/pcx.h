#ifndef PROTO_PCX_H
#define PROTO_PCX_H

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
#ifndef INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif
#ifndef INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * DTClassBase;
 #else
  extern struct ClassBase * DTClassBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/pcx.h>
 #ifdef __USE_INLINE__
  #include <inline4/pcx.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_PCX_PROTOS_H
  #define CLIB_PCX_PROTOS_H 1
 #endif /* CLIB_PCX_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct DTClassIFace *IDTClass;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_PCX_PROTOS_H
  #include <clib/pcx_protos.h>
 #endif /* CLIB_PCX_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/pcx.h>
  #else
   #include <ppcinline/pcx.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/pcx_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/pcx_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_PCX_H */
