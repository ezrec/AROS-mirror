// IconObject.h
// $Date$
// $Revision$
//----------------------------------------------------------------------------

#ifndef ICONOBJ_H
#define	ICONOBJ_H

#include <exec/types.h>
#include <datatypes/iconobject.h>
#include <workbench/workbench.h>

#include "iconobject_base.h"
#include "iconnode.h"

#include <defs.h>

//----------------------------------------------------------------------------

extern struct ExecBase *SysBase;
extern struct IntuitionBase *IntuitionBase;
extern T_UTILITYBASE UtilityBase;
extern struct DosLibrary * DOSBase;

//----------------------------------------------------------------------------

#define	LIB_VERSION	40
#define	LIB_REVISION	8

extern char ALIGNED libName[];
extern char ALIGNED libIdString[];

//-----------------------------------------------------------------------------

#define	MEMPOOL_MEMFLAGS	MEMF_PUBLIC
#define	MEMPOOL_PUDDLESIZE	8192
#define	MEMPOOL_THRESHSIZE	8192

//-----------------------------------------------------------------------------

BOOL IconObjectInit(struct IconObjectBase *IconObjectBase);
BOOL IconObjectOpen(struct IconObjectBase *IconObjectBase);
void IconObjectCleanup(struct IconObjectBase *IconObjectBase);

LIBFUNC_P3_PROTO(struct Iconobject *, LIBNewIconObject,
	A0, APTR, Name,
	A1, CONST struct TagItem *, Taglist,
	A6, struct IconObjectBase *, IconObjectBase, 5);
LIBFUNC_P2_PROTO(void, LIBDisposeIconObject,
	A0, struct Iconobject *, iconobject,
	A6, struct IconObjectBase *, IconObjectBase, 6);
LIBFUNC_P3_PROTO(struct Iconobject *, LIBGetDefIconObject,
	D0, ULONG, IconType,
	A0, CONST struct TagItem *, TagList,
	A6, struct IconObjectBase *, IconObjectBase, 7);
LIBFUNC_P4_PROTO(LONG, LIBPutIconObject,
	A0, struct Iconobject *, iconobject,
	A1, APTR, path,
	A2, CONST struct TagItem *, TagList,
	A6, struct IconObjectBase *, IconObjectBase, 8);
LIBFUNC_P2_PROTO(ULONG, LIBIsIconName,
	A0, const char *, filename,
	A6, struct IconObjectBase *, IconObjectBase, 9);
LIBFUNC_P3_PROTO(struct Iconobject *, LIBConvert2IconObjectA,
	A0, struct DiskObject *, diskobject,
	A1, const struct TagItem *, TagList,
	A6, struct IconObjectBase *, IconObjectBase, 11);
LIBFUNC_P2_PROTO(struct Iconobject *, LIBConvert2IconObject,
	A0, struct DiskObject *, diskobject,
	A6, struct IconObjectBase *, IconObjectBase, 10);

//----------------------------------------------------------------------------

// Debugging...
#define	d(x)	;
#define	d1(x)	;
#define	d2(x)	x;


// aus debug.lib
#ifdef __AROS__
#include <clib/arossupport_protos.h>
#define KPrintF kprintf
#else
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);
#endif

//----------------------------------------------------------------------------

#endif /* ICONOBJ_H */


