/* Automatically generated header! Do not edit! */

#ifndef _INLINE_CIA_H
#define _INLINE_CIA_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#define AddICRVector(___resource, ___iCRBit, ___interrupt) __AddICRVector_WB(CIA_BASE_NAME, ___resource, ___iCRBit, ___interrupt)
#define __AddICRVector_WB(___base, ___resource, ___iCRBit, ___interrupt) \
	AROS_LC2(struct Interrupt *, AddICRVector, \
	AROS_LCA(WORD, (___iCRBit), D0), \
	AROS_LCA(struct Interrupt *, (___interrupt), A1), \
	struct Library *, ___resource, 1, Cia)

#define RemICRVector(___resource, ___iCRBit, ___interrupt) __RemICRVector_WB(CIA_BASE_NAME, ___resource, ___iCRBit, ___interrupt)
#define __RemICRVector_WB(___base, ___resource, ___iCRBit, ___interrupt) \
	AROS_LC2NR(VOID, RemICRVector, \
	AROS_LCA(WORD, (___iCRBit), D0), \
	AROS_LCA(struct Interrupt *, (___interrupt), A1), \
	struct Library *, ___resource, 2, Cia)

#define AbleICR(___resource, ___mask) __AbleICR_WB(CIA_BASE_NAME, ___resource, ___mask)
#define __AbleICR_WB(___base, ___resource, ___mask) \
	AROS_LC1(WORD, AbleICR, \
	AROS_LCA(WORD, (___mask), D0), \
	struct Library *, ___resource, 3, Cia)

#define SetICR(___resource, ___mask) __SetICR_WB(CIA_BASE_NAME, ___resource, ___mask)
#define __SetICR_WB(___base, ___resource, ___mask) \
	AROS_LC1(WORD, SetICR, \
	AROS_LCA(WORD, (___mask), D0), \
	struct Library *, ___resource, 4, Cia)

#endif /* !_INLINE_CIA_H */
