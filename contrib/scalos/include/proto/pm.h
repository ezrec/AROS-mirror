#ifndef POPUPMENU_PROTO_H
#define POPUPMENU_PROTO_H

#ifdef __AROS__
#error You must include AROS headers
#endif

#include <exec/types.h>
#include <clib/pm_protos.h>
extern struct PopupMenuBase	*PopupMenuBase;
#ifdef __amigaos4__
#include <interfaces/pm.h>
extern struct PopupMenuIFace *IPopupMenu;
#endif

#if defined(__amigaos4__)
	#ifdef __USE_INLINE__
		#include <inline4/pm.h>
	#endif
#elif defined(__GNUC__)
	#ifdef __AROS__
		#include <defines/pm.h>
	#else
		#include <inline/pm.h>
	#endif
#elif defined(VBCC)
	#include <inline/pm_protos.h>
#else
	#include <pragmas/pm_pragmas.h>
#endif

#endif
