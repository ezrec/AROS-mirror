// PNGIconProto.h
// $Date$
// $Revision$


#ifndef PNGICONPROTO_H
#define	PNGICONPROTO_H

//-----------------------------------------------------------------------------

// defined in PNGIconDt.c
APTR MyAllocVecPooled(size_t Size);
void MyFreeVecPooled(APTR mem);

//-----------------------------------------------------------------------------

// defined in LoadPngIcon.c
BOOL LoadPngIcon(struct InstanceData *inst, CONST_STRPTR filename, BPTR IconFh, const struct WBArg *ObjectLocation);
void HandleIconHunk(struct InstanceData *inst, const UBYTE *dataptr, size_t Length);

//-----------------------------------------------------------------------------

// defined in GetPNGPicture.c
BOOL GetPngPicture(struct InstanceData *inst, BPTR file,
	ULONG SigBytesRead, struct ARGBHeader *argbh);

//-----------------------------------------------------------------------------

// defined in GenTemplate.c
BOOL GenMask(struct InstanceData *inst, ULONG width, ULONG height);
BOOL GenAlpha(struct InstanceData *inst, struct ExtGadget *gg, ULONG width, ULONG height);

//-----------------------------------------------------------------------------

// defined in SavePngIcon.c
ULONG SavePngIcon(struct InstanceData *inst, CONST_STRPTR name,
	struct ARGBHeader *NormalARGBImage, struct ARGBHeader *SelectedARGBImage);

//-----------------------------------------------------------------------------

// defined in libz
//int uncompress(UBYTE *dest, ULONG *destLen, UBYTE *source, ULONG sourceLen);

//-----------------------------------------------------------------------------

// defined in  mempools.lib

extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);

//-----------------------------------------------------------------------------

// Defined in debug.lib
#ifdef __AROS__
#include <clib/arossupport_protos.h>
#define KPrintF kprintf
#else
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);
#endif

//-----------------------------------------------------------------------------

#endif /* PNGICONPROTO_H */
