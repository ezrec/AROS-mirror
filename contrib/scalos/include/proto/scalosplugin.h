#ifndef PROTO_SCALOSPLUGIN_H
#define PROTO_SCALOSPLUGIN_H

#include <exec/types.h>
extern struct Library *ScalosPluginBase ;
#ifdef __amigaos4__
#include <interfaces/scalosplugin.h>
extern struct ScalosPluginIFace *IScalosPlugin;
#endif

#include <clib/scalosplugin_protos.h>

#ifdef __amigaos4__
	#ifdef __USE_INLINE__
		#include <inline4/scalosplugin.h>
	#endif
#elif defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/scalosplugin.h>
	#else
		#include <inline/scalosplugin.h>
	#endif
#elif defined(VBCC)
	#include <inline/scalosplugin_protos.h>
#else
	#include <pragmas/scalosplugin_pragmas.h>
#endif

#endif
