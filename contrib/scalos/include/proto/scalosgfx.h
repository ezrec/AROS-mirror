#ifndef PROTO_SCALOSGFX_H
#define PROTO_SCALOSGFX_H

#include <exec/types.h>
#include <scalos/scalos.h>
extern struct ScalosGfxBase *ScalosGfxBase ;
#ifdef __amigaos4__
#include <interfaces/scalosgfx.h>
extern struct ScalosGfxIFace *IScalosGfx;
#endif /* __amigaos4__ */

#include <clib/scalosgfx_protos.h>

#ifdef __amigaos4__
	#ifdef __USE_INLINE__
		#include <inline4/scalosgfx.h>
	#endif
#elif defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/scalosgfx.h>
	#else
		#include <inline/scalosgfx.h>
	#endif
#elif defined(VBCC)
	#include <inline/scalosgfx_protos.h>
#else
	#include <pragmas/scalosgfx_pragmas.h>
#endif

#endif  /* PROTO_SCALOSGFX_H */

