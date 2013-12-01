#ifndef _INLINE_SCALOSPREFSPLUGIN_H
#define _INLINE_SCALOSPREFSPLUGIN_H

#ifndef CLIB_SCALOSPREFSPLUGIN_PROTOS_H
#define CLIB_SCALOSPREFSPLUGIN_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#include <exec/types.h>

#ifndef SCALOSPREFSPLUGIN_BASE_NAME
#define SCALOSPREFSPLUGIN_BASE_NAME ScalosPrefsPluginBase
#endif

#define SCAGetPrefsInfo(which) \
	LP1(0x1e, ULONG, SCAGetPrefsInfo, LONG, which, d0, \
	, SCALOSPREFSPLUGIN_BASE_NAME)

#endif /*  _INLINE_SCALOSPREFSPLUGIN_H  */
