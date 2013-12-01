/* Automatically generated header! Do not edit! */

#ifndef _INLINE_SCALOSMENUPLUGIN_LIB_SFD_H
#define _INLINE_SCALOSMENUPLUGIN_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef SCALOSMENUPLUGIN_LIB_SFD_BASE_NAME
#define SCALOSMENUPLUGIN_LIB_SFD_BASE_NAME ScalosMenuPluginBase
#endif /* !SCALOSMENUPLUGIN_LIB_SFD_BASE_NAME */

#define SCAMenuFunction(___wt, ___in) __SCAMenuFunction_WB(SCALOSMENUPLUGIN_LIB_SFD_BASE_NAME, ___wt, ___in)
#define __SCAMenuFunction_WB(___base, ___wt, ___in) \
	AROS_LC2NR(VOID, SCAMenuFunction, \
	AROS_LCA(struct ScaWindowTask *, (___wt), A0), \
	AROS_LCA(struct ScaIconNode *, (___in), A1), \
	struct Library *, (___base), 5, Scalosmenuplugin_lib_sfd)

#endif /* !_INLINE_SCALOSMENUPLUGIN_LIB_SFD_H */
