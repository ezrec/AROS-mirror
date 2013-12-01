#ifndef _INLINE_SCALOSFILETYPEPLUGIN_H
#define _INLINE_SCALOSFILETYPEPLUGIN_H

#ifndef CLIB_SCALOSFILETYPEPLUGIN_PROTOS_H
#define CLIB_SCALOSFILETYPEPLUGIN_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  SCALOS_SCALOS_H
#include <scalos/scalos.h>
#endif

#ifndef SCALOSFILETYPEPLUGIN_BASE_NAME
#define SCALOSFILETYPEPLUGIN_BASE_NAME ScalosFileTypePluginBase
#endif

#define SCAToolTipInfoString(ttshd, args) \
	LP2(0x1e, STRPTR, SCAToolTipInfoString, struct ScaToolTipInfoHookData *, ttshd, a0, CONST_STRPTR, args, a1, \
	, SCALOSFILETYPEPLUGIN_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define SCAToolTipInfoStringArgs(ttshd, tags...) \
	({ULONG _tags[] = {tags}; SCAToolTipInfoString((ttshd), (CONST_STRPTR) _tags);})
#endif

#endif /*  _INLINE_SCALOSFILETYPEPLUGIN_H  */
