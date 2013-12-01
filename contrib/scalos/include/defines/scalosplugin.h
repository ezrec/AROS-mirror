/* Automatically generated header! Do not edit! */

#ifndef _INLINE_SCALOSPLUGIN_LIB_SFD_H
#define _INLINE_SCALOSPLUGIN_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef SCALOSPLUGIN_LIB_SFD_BASE_NAME
#define SCALOSPLUGIN_LIB_SFD_BASE_NAME ScalosPluginBase
#endif /* !SCALOSPLUGIN_LIB_SFD_BASE_NAME */

#define SCAGetClassInfo() __SCAGetClassInfo_WB(SCALOSPLUGIN_LIB_SFD_BASE_NAME)
#define __SCAGetClassInfo_WB(___base) \
	AROS_LC0(const struct ScaClassInfo *, SCAGetClassInfo, \
	struct Library *, (___base), 5, Scalosplugin_lib_sfd)

#endif /* !_INLINE_SCALOSPLUGIN_LIB_SFD_H */
