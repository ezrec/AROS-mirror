#ifndef PROTO_CYBERGRAPHICS_H
#define PROTO_CYBERGRAPHICS_H 1

#ifdef __AROS__
#error You must include AROS headers
#endif

#include <exec/types.h>
extern struct Library *CyberGfxBase;
#ifdef __amigaos4__
#include <interfaces/cybergraphics.h>
extern struct CyberGfxIFace *ICyberGfx;
#endif /* __amigaos4__ */

#include <clib/cybergraphics_protos.h>

#ifdef __amigaos4__
	#ifdef __USE_INLINE__
		#include <inline4/cybergraphics.h>
	#endif
#elif defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/cybergraphics.h>
	#else
		#include <inline/cybergraphics.h>
	#endif
#elif defined(VBCC)
	#include <inline/cybergraphics_protos.h>
#else
	#include <pragmas/cybergraphics_pragmas.h>
#endif

#endif /* !PROTO_CYBERGRAPHICS_H */
