#ifndef _INLINE_SCALOSPLUGIN_H
#define _INLINE_SCALOSPLUGIN_H

#ifndef CLIB_SCALOSPLUGIN_PROTOS_H
#define CLIB_SCALOSPLUGIN_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#include <exec/types.h>

#ifndef SCALOSPLUGIN_BASE_NAME
#define SCALOSPLUGIN_BASE_NAME ScalosPluginBase
#endif

#define SCAGetClassInfo() \
	LP0(0x1e, const struct ScaClassInfo *, SCAGetClassInfo, \
	, SCALOSPLUGIN_BASE_NAME)

#endif /*  _INLINE_SCALOSPLUGIN_H  */
