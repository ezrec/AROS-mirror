#ifndef PROTO_SCALOSPREVIEWPLUGIN_H
#define PROTO_SCALOSPREVIEWPLUGIN_H

#include <exec/types.h>
extern struct Library *ScalosPreviewPluginBase ;
#ifdef __amigaos4__
#include <interfaces/scalospreviewplugin.h>
extern struct ScalosPreviewPluginIFace *IScalosPreviewPlugin;
#endif

#include <clib/scalospreviewplugin_protos.h>

#ifdef __amigaos4__
	#ifdef __USE_INLINE__
		#include <inline4/scalospreviewplugin.h>
	#endif
#elif defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/scalospreviewplugin.h>
	#else
		#include <inline/scalospreviewplugin.h>
	#endif
#elif defined(VBCC)
	#include <inline/scalospreviewplugin_protos.h>
#else
	#include <pragmas/scalospreviewplugin_pragmas.h>
#endif

#endif  /* PROTO_SCALOSPREVIEWPLUGIN_H */
