/* Automatically generated header! Do not edit! */

#ifndef _INLINE_SCALOSPREFSPLUGIN_LIB_SFD_H
#define _INLINE_SCALOSPREFSPLUGIN_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef SCALOSPREFSPLUGIN_LIB_SFD_BASE_NAME
#define SCALOSPREFSPLUGIN_LIB_SFD_BASE_NAME ScalosPrefsPluginBase
#endif /* !SCALOSPREFSPLUGIN_LIB_SFD_BASE_NAME */

#define SCAGetPrefsInfo(___which) __SCAGetPrefsInfo_WB(SCALOSPREFSPLUGIN_LIB_SFD_BASE_NAME, ___which)
#define __SCAGetPrefsInfo_WB(___base, ___which) \
	AROS_LC1(ULONG, SCAGetPrefsInfo, \
	AROS_LCA(LONG, (___which), D0), \
	struct Library *, (___base), 5, Scalosprefsplugin_lib_sfd)

#endif /* !_INLINE_SCALOSPREFSPLUGIN_LIB_SFD_H */
