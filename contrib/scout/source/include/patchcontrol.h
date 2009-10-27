#ifndef PATCHCONTROL_H
#define PATCHCONTROL_H 1

#define PATCHRES_NAME       "patch.resource"

struct PatchControlNode {
    struct PatchControlNode *pcn_Next;
    ULONG pcn_unknown0;
    struct Library *pcn_Library;
    LONG pcn_Offset;
    APTR pcn_FunctionAddress;
    ULONG pcn_unknown1;
    UBYTE *pcn_TaskName;
};

extern struct Library *PatchBase;

APTR GetPatchList( void );
void FreePatchList( APTR list );

#if !defined(__MORPHOS__)

#pragma libcall PatchBase GetPatchList 6 00
#pragma libcall PatchBase FreePatchList c 801

#else

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#define GetPatchList() \
	LP0(6, APTR, GetPatchList, \
		, PatchBase, 0, 0, 0, 0, 0, 0)
#define FreePatchList(__p0) \
	LP1NR(12, FreePatchList, \
		APTR, __p0, a0, \
		, PatchBase, 0, 0, 0, 0, 0, 0)

#endif

#endif /* PATCHCONTROL_H */

