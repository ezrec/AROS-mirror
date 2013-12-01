// FontSampleMCC.h
// $Date$
// $Revision$


#ifndef FONTSAMPLE_MCC_H
#define	FONTSAMPLE_MCC_H

/* ------------------------------------------------------------------------- */

#include <defs.h>

/* ------------------------------------------------------------------------- */

extern struct MUI_CustomClass *FontSampleClass;

#ifdef __AROS__
#define FontSampleObject BOOPSIOBJMACRO_START(FontSampleClass->mcc_Class)
#else
#define FontSampleObject NewObject(FontSampleClass->mcc_Class, 0
#endif

struct MUI_CustomClass *InitFontSampleClass(void);
void CleanupFontSampleClass(struct MUI_CustomClass *mcc);

/* ------------------------------------------------------------------------- */

#define	MAX_TTFONTDESC	256

/* ------------------------------------------------------------------------- */

#define	MUIA_FontSample_TTFontDesc		0x82457651	/* CONST_STRPTR */
#define	MUIA_FontSample_DemoString		0x82457652	/* CONST_STRPTR */
#define	MUIA_FontSample_StdFontDesc		0x82457653	/* CONST_STRPTR */
#define	MUIA_FontSample_Antialias		0x82457654	/* ULONG */
#define	MUIA_FontSample_Gamma			0x82457655	/* ULONG */
#define MUIA_FontSample_HAlign                  0x82457656	/* ULONG */
#define MUIA_FontSample_VAlign                  0x82457657	/* ULONG */

//--------------------------------------------------------------------

#define	FONTSAMPLE_HALIGN_LEFT			1
#define	FONTSAMPLE_HALIGN_CENTER		2
#define	FONTSAMPLE_HALIGN_RIGHT			3

#define	FONTSAMPLE_VALIGN_TOP			1
#define	FONTSAMPLE_VALIGN_CENTER		2
#define	FONTSAMPLE_VALIGN_BOTTOM		3

//--------------------------------------------------------------------


// from debug.lib
extern int kprintf(const char *fmt, ...);

//--------------------------------------------------------------------

#define	d1(x)		;
#define	d2(x)		x;

/* ------------------------------------------------------------------------- */

#endif /* FONTSAMPLE_MCC_H */
