// BitMapMCC.h
// $Date$
// $Revision$


#ifndef BITMAP_MCC_H
#define BITMAP_MCC_H

/* ------------------------------------------------------------------------- */

#include <defs.h>

/* ------------------------------------------------------------------------- */

struct MUI_CustomClass *InitBitMappicClass(void);
void CleanupBitMappicClass(struct MUI_CustomClass *mcc);

/* ------------------------------------------------------------------------- */

extern struct MUI_CustomClass *DataTypesImageClass;

#ifdef __AROS__
#define DataTypesImageObject BOOPSIOBJMACRO_START(DataTypesImageClass->mcc_Class)
#else
#define DataTypesImageObject NewObject(DataTypesImageClass->mcc_Class, 0
#endif

/* ------------------------------------------------------------------------- */

#define	MUIA_BITMAPPIC_TAGBASE		0x8042a875

#define MUIA_ScaBitMappic_BitMap	(MUIA_BITMAPPIC_TAGBASE+1)	// [I.G] struct BitMap *
#define MUIA_ScaBitMappic_ColorTable	(MUIA_BITMAPPIC_TAGBASE+2)  	// [I.G] ULONG *
#define MUIA_ScaBitMappic_Width		(MUIA_BITMAPPIC_TAGBASE+3)	// [I.G] ULONG
#define MUIA_ScaBitMappic_Height	(MUIA_BITMAPPIC_TAGBASE+4)	// [I.G] ULONG
#define MUIA_ScaBitMappic_Screen	(MUIA_BITMAPPIC_TAGBASE+5)	// [I.G] struct Screen *
#define MUIA_ScaBitmappic_BitMapArray	(MUIA_BITMAPPIC_TAGBASE+6)	// [I.G] UBYTE *

/* ------------------------------------------------------------------------- */

#endif /* BITMAP_MCC_H */
