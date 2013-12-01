#ifndef _INLINE_SCALOSMENUPLUGIN_H
#define _INLINE_SCALOSMENUPLUGIN_H

#ifndef CLIB_SCALOSMENUPLUGIN_PROTOS_H
#define CLIB_SCALOSMENUPLUGIN_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  SCALOS_SCALOS_H
#include <scalos/scalos.h>
#endif

#ifndef SCALOSMENUPLUGIN_BASE_NAME
#define SCALOSMENUPLUGIN_BASE_NAME ScalosMenuPluginBase
#endif

#define SCAMenuFunction(wt, in) \
	LP2NR(0x1e, SCAMenuFunction, struct ScaWindowTask *, wt, a0, struct ScaIconNode *, in, a1, \
	, SCALOSMENUPLUGIN_BASE_NAME)

#endif /*  _INLINE_SCALOSMENUPLUGIN_H  */
