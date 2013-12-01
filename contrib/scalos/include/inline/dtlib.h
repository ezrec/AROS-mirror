#ifndef _INLINE_DTLIB_H
#define _INLINE_DTLIB_H

#ifndef CLIB_DTLIB_PROTOS_H
#define CLIB_DTLIB_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif

#ifndef DTLIB_BASE_NAME
#define DTLIB_BASE_NAME DtLibBase
#endif

#define ObtainInfoEngine(libBase) \
	LP1(0x1e, Class *, ObtainInfoEngine, struct Library *, libBase, a0, \
	, DTLIB_BASE_NAME)

#endif /*  _INLINE_DTLIB_H  */
