#ifndef  PROTO_SCALOSPREFSPLUGIN_H
#define  PROTO_SCALOSPREFSPLUGIN_H

#include <exec/types.h>
extern struct Library *ScalosPrefsPluginBase ;
#ifdef __amigaos4__
#include <interfaces/scalosprefsplugin.h>
extern struct ScalosPrefsPluginIFace *IScalosPrefsPlugin;
#endif

#include <clib/scalosprefsplugin_protos.h>

#ifdef __amigaos4__
	#ifdef __USE_INLINE__
		#include <inline4/scalosprefsplugin.h>
	#endif
#elif defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/scalosprefsplugin.h>
	#else
		#include <inline/scalosprefsplugin.h>
	#endif
#elif defined(VBCC)
	#include <inline/scalosprefsplugin_protos.h>
#else
	#include <pragmas/scalosprefsplugin_pragmas.h>
#endif

#endif   /* PROTO_SCALOSPREFSPLUGIN_H */
