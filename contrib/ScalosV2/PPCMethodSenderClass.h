
#ifndef SCALOS_H
#include "Scalos.h"
#endif /* SCALOS_H */

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

struct MethodSenderInst
{
	ULONG	senderid;				// to find out that this object is a sender
	Object	*destobject;
	ULONG	flags;					// see below
};

#define SCALOS_SENDERID		MAKE_ID('S','S','E','N')

