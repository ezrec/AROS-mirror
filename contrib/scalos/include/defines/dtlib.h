/* Automatically generated header! Do not edit! */

#ifndef _INLINE_DTLIB_LIB_SFD_H
#define _INLINE_DTLIB_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef DTLIB_LIB_SFD_BASE_NAME
#define DTLIB_LIB_SFD_BASE_NAME DtLibBase
#endif /* !DTLIB_LIB_SFD_BASE_NAME */

#define ObtainInfoEngine(___libBase) __ObtainInfoEngine_WB(DTLIB_LIB_SFD_BASE_NAME, ___libBase)
#define __ObtainInfoEngine_WB(___base, ___libBase) \
	AROS_LC1(Class *, ObtainInfoEngine, \
	AROS_LCA(struct Library *, (___libBase), A0), \
	struct Library *, (___base), 5, Dtlib_lib_sfd)

#endif /* !_INLINE_DTLIB_LIB_SFD_H */
