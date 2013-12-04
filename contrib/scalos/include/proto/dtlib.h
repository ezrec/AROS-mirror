#ifndef PROTO_SCALOSPLUGIN_H
#define PROTO_SCALOSPLUGIN_H

#ifdef __AROS__
#error You must include AROS headers
#endif

#include <exec/types.h>
extern struct Library *DtLibBase ;

#include <clib/dtlib_protos.h>

#if defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/dtlib.h>
	#else
		#include <inline/dtlib.h>
	#endif
#elif defined(VBCC)
	#include <inline/dtlib_protos.h>
#else
	#include <pragmas/dtlib_pragmas.h>
#endif

#endif
