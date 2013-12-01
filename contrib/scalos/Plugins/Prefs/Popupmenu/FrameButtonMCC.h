// FrameButtonMCC.h
// $Date$
// $Revision$


#ifndef FRAMEBUTTON_MCC_H
#define FRAMEBUTTON_MCC_H

/* ------------------------------------------------------------------------- */

#include <defs.h>

/* ------------------------------------------------------------------------- */

struct MUI_CustomClass *InitFrameButtonClass(void);
void CleanupFrameButtonClass(struct MUI_CustomClass *mcc);

/* ------------------------------------------------------------------------- */

extern struct MUI_CustomClass *FrameButtonClass;

#ifdef __AROS__
#define FrameButtonObject BOOPSIOBJMACRO_START(FrameButtonClass->mcc_Class)
#else
#define FrameButtonObject NewObject(FrameButtonClass->mcc_Class, 0
#endif

/* ------------------------------------------------------------------------- */

#define	PMP_MENUBORDER_NONE		999

/* ------------------------------------------------------------------------- */

#if !defined(HALFSHINEPEN)
#define	HALFSHINEPEN		SHINEPEN
#define	HALFSHADOWPEN		SHADOWPEN
#endif /* defined(HALFSHINEPEN) */

/* ------------------------------------------------------------------------- */

#define	MUIA_FRAMEBUTTON_TAGBASE	0x8042a877

#define MUIA_ScaFrameButton_FrameType	(MUIA_FRAMEBUTTON_TAGBASE+1)	// [I.G] ULONG
#define	MUIA_ScaFrameButton_Selected	(MUIA_FRAMEBUTTON_TAGBASE+2)	// [I.G] ULONG
#define	MUIA_ScaFrameButton_Raised	(MUIA_FRAMEBUTTON_TAGBASE+3)	// [I.G] ULONG

/* ------------------------------------------------------------------------- */

#endif /* FRAMEBUTTON_MCC_H */
