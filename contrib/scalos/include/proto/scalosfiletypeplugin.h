#ifndef PROTO_SCALOSFILETYPEPLUGIN_H
#define PROTO_SCALOSFILETYPEPLUGIN_H

#include <exec/types.h>
extern struct Library *ScalosFileTypePluginBase ;
#ifdef __amigaos4__
#include <interfaces/scalosfiletypeplugin.h>
extern struct ScalosFileTypePluginIFace *IScalosFileTypePlugin;
#endif

#include <clib/scalosfiletypeplugin_protos.h>

#ifdef __amigaos4__
	#ifdef __USE_INLINE__
		#include <inline4/scalosfiletypeplugin.h>
	#endif
#elif defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/scalosfiletypeplugin.h>
	#else
		#include <inline/scalosfiletypeplugin.h>
	#endif
#elif defined(VBCC)
	#include <inline/scalosfiletypeplugin_protos.h>
#else
	#include <pragmas/scalosfiletypeplugin_pragmas.h>
#endif

#endif
