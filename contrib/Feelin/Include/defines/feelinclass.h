/* Automatically generated header! Do not edit! */

#ifndef _INLINE_FEELINCLASS_H
#define _INLINE_FEELINCLASS_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef FEELINCLASS_BASE_NAME
#define FEELINCLASS_BASE_NAME FeelinClassBase
#endif /* !FEELINCLASS_BASE_NAME */

#define F_Query(___Which, ___Feelin) \
	AROS_LC2(struct TagItem *, F_Query, \
	AROS_LCA(ULONG, (___Which), D0), \
	AROS_LCA(struct FeelinBase *, (___Feelin), A0), \
	struct Library *, FEELINCLASS_BASE_NAME, 5, /* s */)

#endif /* !_INLINE_FEELINCLASS_H */
