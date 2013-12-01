/* Automatically generated header! Do not edit! */

#ifndef _INLINE_SCALOSFILETYPEPLUGIN_LIB_SFD_H
#define _INLINE_SCALOSFILETYPEPLUGIN_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef SCALOSFILETYPEPLUGIN_LIB_SFD_BASE_NAME
#define SCALOSFILETYPEPLUGIN_LIB_SFD_BASE_NAME ScalosFileTypePluginBase
#endif /* !SCALOSFILETYPEPLUGIN_LIB_SFD_BASE_NAME */

#define SCAToolTipInfoString(___ttshd, ___args) __SCAToolTipInfoString_WB(SCALOSFILETYPEPLUGIN_LIB_SFD_BASE_NAME, ___ttshd, ___args)
#define __SCAToolTipInfoString_WB(___base, ___ttshd, ___args) \
	AROS_LC2(STRPTR, SCAToolTipInfoString, \
	AROS_LCA(struct ScaToolTipInfoHookData *, (___ttshd), A0), \
	AROS_LCA(CONST_STRPTR, (___args), A1), \
	struct Library *, (___base), 5, Scalosfiletypeplugin_lib_sfd)

#endif /* !_INLINE_SCALOSFILETYPEPLUGIN_LIB_SFD_H */
