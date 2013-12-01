// iconobject_base.h
// $Date$
// $Revision$


#ifndef ICONOBJBASE_H
#define	ICONOBJBASE_H

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>

struct IconObjectBase
{
	struct Library iob_LibNode;

	struct SegList *iob_SegList;

	APTR iob_AmigaIcon;
	struct SignalSemaphore  iob_Semaphore;

	struct List iob_ClassList;

	BPTR iob_dtPathLock;

	UWORD iob_pad;
};

#endif /* ICONOBJBASE_H */
