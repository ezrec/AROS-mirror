#ifndef PROTO_ICONOBJECT_H
#define PROTO_ICONOBJECT_H
#include <exec/types.h>
#include <exec/libraries.h>

#ifdef __AROS__
#error You must include AROS headers
#endif

extern struct Library *IconobjectBase;
#ifdef __amigaos4__
#include <interfaces/iconobject.h>
extern struct IconobjectIFace *IIconobject;
#endif

#include <clib/iconobject_protos.h>

#ifdef __amigaos4__
	#ifdef __USE_INLINE__
		#include <inline4/iconobject.h>
	#endif
#elif defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/iconobject.h>
	#else
		#include <inline/iconobject.h>
	#endif
#elif defined(VBCC)
	#include <inline/iconobject_protos.h>
#else
	#include <pragmas/iconobject_pragmas.h>
#endif
#endif
