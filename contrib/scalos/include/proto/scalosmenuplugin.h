#ifndef PROTO_SCALOSMENUPLUGIN_H
#define PROTO_SCALOSMENUPLUGIN_H

#ifdef __AROS__
#error You must include AROS headers
#endif

#include <exec/types.h>
extern struct Library *ScalosMenuPluginBase ;
#ifdef __amigaos4__
#include <interfaces/scalosmenuplugin.h>
extern struct ScalosMenuPluginIFace *IScalosMenuPlugin;
#endif

#include <clib/scalosmenuplugin_protos.h>

#ifdef __amigaos4__
	#ifdef __USE_INLINE__
		#include <inline4/scalosmenuplugin.h>
	#endif
#elif defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/scalosmenuplugin.h>
	#else
		#include <inline/scalosmenuplugin.h>
	#endif
#elif defined(VBCC)
	#include <inline/scalosmenuplugin_protos.h>
#else
	#include <pragmas/scalosmenuplugin_pragmas.h>
#endif

#endif
