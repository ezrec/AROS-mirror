#ifndef _PROTO_NEWICON_H
#define _PROTO_NEWICON_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_NEWICON_PROTOS_H) && !defined(__GNUC__)
#include <clib/newicon_protos.h>
#endif
#ifndef LIBRARIES_NEWICON_H
#include <libraries/newicon.h>
#endif

#ifndef __NOLIBBASE__
extern struct NewIconBase *NewIconBase ;
#endif

#if defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/newicon.h>
	#else
		#include <inline/newicon.h>
	#endif
#elif defined(VBCC)
	#include <inline/dtlib_protos.h>
#else
	#include <pragmas/newicon_pragmas.h>
#endif

#endif	/*  _PROTO_NEWICON_H  */
