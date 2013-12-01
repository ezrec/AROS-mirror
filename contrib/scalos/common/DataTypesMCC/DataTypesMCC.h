// DataTypesMCC.h
// $Date$
// $Revision$


#ifndef DATATYPES_MCC_H
#define	DATATYPES_MCC_H

/* ------------------------------------------------------------------------- */

#include <defs.h>

/* ------------------------------------------------------------------------- */

struct MUI_CustomClass *InitDtpicClass(void);
void CleanupDtpicClass(struct MUI_CustomClass *mcc);

/* ------------------------------------------------------------------------- */

extern struct MUI_CustomClass *DataTypesImageClass;

#ifdef __AROS__
#define DataTypesImageObject BOOPSIOBJMACRO_START(DataTypesImageClass->mcc_Class)
#else
#define DataTypesImageObject NewObject(DataTypesImageClass->mcc_Class, 0
#endif

/* ------------------------------------------------------------------------- */

#define	MUIA_DTPIC_TAGBASE		0x80429875

#define	MUIA_ScaDtpic_Name		(MUIA_DTPIC_TAGBASE+1)	// [IS.] CONST_STRPTR
#define MUIA_ScaDtpic_FailIfUnavailable	(MUIA_DTPIC_TAGBASE+2)	// [I..] ULONG
#define MUIA_ScaDtpic_Tiled		(MUIA_DTPIC_TAGBASE+3)	// [IS.] ULONG

//----------------------------------------------------------------------------

#define	d1(x)	;
#define	d2(x)	x;

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

/* ------------------------------------------------------------------------- */

#endif /* DATATYPES_MCC_H */
