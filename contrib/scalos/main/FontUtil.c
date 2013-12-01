// FontUtil.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#define USE_INLINE_STDARG
#include <proto/ttengine.h>
#undef USE_INLINE_STDARG
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local functions

static void SetFamilyTTFont(struct RastPort *rp, ULONG style, struct TTFontFamily *ttff);
static BOOL FillTTTextAttrFromDesc(struct TTTextAttr *ttta, CONST_STRPTR FontDesc);
static SAVEDS(ULONG) FontFunctionDispatcher(struct Hook *hook, Object *o, Msg msg);

//----------------------------------------------------------------------------

// public data items

struct Hook ScalosFontHook = 
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(FontFunctionDispatcher),  // h_Entry + h_SubEntry
	NULL,					// h_Data
	};

SCALOSSEMAPHORE tteSema;

//----------------------------------------------------------------------------

// local data items

#define	USE_SEMA

//----------------------------------------------------------------------------

BOOL FontUtilInit(void)
{
	ScalosInitSemaphore(&tteSema);

	return TRUE;	// Success
}


SAVEDS(WORD) Scalos_TextLength(struct RastPort *rp, CONST_STRPTR string, WORD Length)
{
	d1(kprintf("%s/%s/%ld: TTEngineBase=%08lx  rp=%08lx\n", __FILE__, __FUNC__, __LINE__, TTEngineBase, rp));

	if (NULL == rp)
		return 0;

	if (TTEngineBase)
		{
		// TT_GetAttrsA()
		STRPTR FontName = NULL;

		TT_GetAttrs(rp, TT_FontName, (ULONG) &FontName,
			TAG_END);

		if (FontName)
			{
			WORD Result;

#ifdef USE_SEMA
			ScalosObtainSemaphore(&tteSema);
#endif
			Result = (WORD) TT_TextLength(rp, (STRPTR) string, Length);
#ifdef USE_SEMA
			ScalosReleaseSemaphore(&tteSema);
#endif

			return Result;
			}
		}

	return TextLength(rp, (STRPTR) string, Length);
}


SAVEDS(ULONG) Scalos_TextFit(struct RastPort *rp, CONST_STRPTR string, WORD Length, 
	struct TextExtent *textExtent, struct TextExtent *constrainingExtent, 
	WORD strDirection, UWORD constrainingBitWidth, UWORD constrainingBitHeight)
{
	ULONG Result;

	d1(kprintf("%s/%s/%ld: TTEngineBase=%08lx  rp=%08lx\n", __FILE__, __FUNC__, __LINE__, TTEngineBase, rp));

	if (NULL == rp)
		return 0;

	if (TTEngineBase)
		{
		// TT_GetAttrsA()
		STRPTR FontName = NULL;

		TT_GetAttrs(rp, TT_FontName, (ULONG) &FontName,
			TAG_END);

		d1(kprintf("%s/%s/%ld: FontName=%08lx\n", __FILE__, __FUNC__, __LINE__, FontName));
		if (FontName)
			{
			d1(kprintf("%s/%s/%ld: textExtent: %ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
				textExtent->te_Extent.MinX, textExtent->te_Extent.MinY,\
				textExtent->te_Extent.MaxX, textExtent->te_Extent.MaxY));
			d1(kprintf("%s/%s/%ld: constrainingExtent=%08lx\n", __FILE__, __FUNC__, __LINE__, constrainingExtent));
			d1(kprintf("%s/%s/%ld: constrainingBitWidth=%ld  constrainingBitHeight=%ld\n", __FILE__, __FUNC__, __LINE__, constrainingBitWidth, constrainingBitHeight));
#ifdef USE_SEMA
			ScalosObtainSemaphore(&tteSema);
#endif
			Result = TT_TextFit(rp,
				(STRPTR) string,
				Length,
				textExtent,
				constrainingExtent,
				strDirection,
				constrainingBitWidth,
				constrainingBitWidth);
#ifdef USE_SEMA
			ScalosReleaseSemaphore(&tteSema);
#endif
			d1(kprintf("%s/%s/%ld: Result=%lu\n", __FILE__, __FUNC__, __LINE__, Result));

			return Result;
			}
		}

	Result = TextFit(rp, (STRPTR) string, Length, textExtent, constrainingExtent, 
		strDirection, constrainingBitWidth, constrainingBitHeight);

	d1(kprintf("%s/%s/%ld: Result=%lu\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


SAVEDS(void) Scalos_TextExtent(struct RastPort *rp, CONST_STRPTR string, WORD Length, struct TextExtent *tExt)
{
	d1(kprintf("%s/%s/%ld: TTEngineBase=%08lx  rp=%08lx\n", __FILE__, __FUNC__, __LINE__, TTEngineBase, rp));

	if (NULL == rp)
		{
		memset(tExt, 0, sizeof(struct TextExtent));
		return;
		}

	if (TTEngineBase)
		{
		// TT_GetAttrsA()
		STRPTR FontName = NULL;

		TT_GetAttrs(rp, TT_FontName, (ULONG) &FontName,
			TAG_END);

		d1(kprintf("%s/%s/%ld: FontName=%08lx\n", __FILE__, __FUNC__, __LINE__, FontName));

		if (FontName)
			{
			d1(kprintf("%s/%s/%ld: FontName=<%s>  rp=%08lx  TextExtent=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, FontName, rp, tExt));

			d1(kprintf("%s/%s/%ld: string=<%s>  Length=%ld\n", __FILE__, __FUNC__, __LINE__, string, Length));

#ifdef USE_SEMA
			ScalosObtainSemaphore(&tteSema);
#endif
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			TT_TextExtent(rp, (STRPTR) string, Length, tExt);

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

#ifdef USE_SEMA
			ScalosReleaseSemaphore(&tteSema);
#endif
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			return;
			}
		}

	TextExtent(rp, (STRPTR) string, Length, tExt);
}


SAVEDS(void) Scalos_Text(struct RastPort *rp, CONST_STRPTR string, WORD Length)
{
	if (rp)
		{
		if (TTEngineBase)
			{
			// TT_GetAttrsA()
			STRPTR FontName = NULL;

			TT_GetAttrs(rp, TT_FontName, (ULONG) &FontName,
				TAG_END);

			d1(kprintf("%s/%s/%ld: FontName=%08lx\n", __FILE__, __FUNC__, __LINE__, FontName));

			if (FontName)
				{
				d1(kprintf("%s/%s/%ld: rp=%08lx  string=<%s>  Length=%ld\n", \
					__FILE__, __FUNC__, __LINE__, rp, string, Length));

#ifdef USE_SEMA
				ScalosObtainSemaphore(&tteSema);
#endif
				TT_Text(rp, (STRPTR) string, Length);
#ifdef USE_SEMA
				ScalosReleaseSemaphore(&tteSema);
#endif

				return;
				}
			}

		Text(rp, (STRPTR) string, Length);
		}
}


SAVEDS(UWORD) Scalos_GetFontHeight(struct RastPort *rp)
{
	if (TTEngineBase && rp)
		{
		STRPTR FontName = NULL;

		// TT_GetAttrsA()
		TT_GetAttrs(rp, TT_FontName, (ULONG) &FontName,
			TAG_END);

		d1(kprintf("%s/%s/%ld: rp=%08lx FontName=%08lx\n", __FILE__, __FUNC__, __LINE__, rp, FontName));

		if (FontName)
			{
			ULONG Height = 0;
			ULONG asc = 0, desc = 0;

			TT_GetAttrs(rp, TT_FontDesignHeight, (ULONG) &Height,
				TT_FontRealAscender, (ULONG) &asc,
				TT_FontRealDescender, (ULONG) &desc,
				TAG_END);

			d1(kprintf("%s/%s/%ld: FontName=<%s>  DesignHeight=%ld\n", __FILE__, __FUNC__, __LINE__, FontName, Height));
			d1(kprintf("%s/%s/%ld: ascender=%ld  descender=%ld\n", __FILE__, __FUNC__, __LINE__, asc, desc));
			return (UWORD) Height;
			}
		}

	return rp->TxHeight;
}


SAVEDS(UWORD) Scalos_GetFontBaseline(struct RastPort *rp)
{
	d1(kprintf("%s/%s/%ld: rp=%08lx\n", __FILE__, __FUNC__, __LINE__, rp));

	if (TTEngineBase && rp)
		{
		STRPTR FontName = NULL;

		// TT_GetAttrsA()
		TT_GetAttrs(rp, TT_FontName, (ULONG) &FontName,
			TAG_END);


		d1(kprintf("%s/%s/%ld: FontName=%08lx\n", __FILE__, __FUNC__, __LINE__, FontName));

		if (FontName)
			{
			ULONG Baseline = 0;

			TT_GetAttrs(rp, TT_FontAccentedAscender, (ULONG) &Baseline,
				TAG_END);

			return (UWORD) (Baseline - 2);
			}
		}

	d1(kprintf("%s/%s/%ld: TxBaseline=%ld  tf_Baseline=%ld\n", __FILE__, __FUNC__, __LINE__, rp->TxBaseline, rp->Font->tf_Baseline));

	return (UWORD) rp->Font->tf_Baseline;
}


SAVEDS(void) Scalos_SetFontTransparency(struct RastPort *rp, ULONG Transparency)
{
	if (TTEngineBase && rp)
		{
		STRPTR FontName = NULL;

		// TT_GetAttrsA()
		TT_GetAttrs(rp, TT_FontName, (ULONG) &FontName,
			TAG_END);

		if (FontName)
			{
			d1(kprintf("%s/%s/%ld: rp=%08lx  FamilyName=<%s>  Transparency=%ld\n", \
				__FILE__, __FUNC__, __LINE__, rp, FontName, Transparency));

#ifdef USE_SEMA
			ScalosObtainSemaphore(&tteSema);
#endif
			// TT_SetAttrsA()
			TT_SetAttrs(rp, TT_Transparency, Transparency,
				TAG_END);

#ifdef USE_SEMA
			ScalosReleaseSemaphore(&tteSema);
#endif
			}
		}
}


SAVEDS(ULONG) Scalos_SetSoftStyle(struct RastPort *rp, ULONG style, ULONG enable, struct TTFontFamily *ttff)
{
	if (TTEngineBase)
		{
		STRPTR FontName = NULL;

		// TT_GetAttrsA()
		TT_GetAttrs(rp, TT_FontName, (ULONG) &FontName,
			TAG_END);

		if (FontName)
			{
			d1(kprintf("%s/%s/%ld: rp=%08lx  FamilyName=<%s>  style=%08lx enable=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, rp, FontName, style, enable));

#ifdef USE_SEMA
			ScalosObtainSemaphore(&tteSema);
#endif
			TT_SetAttrs(rp,
				TT_SoftStyle, ((style & enable) == FSF_UNDERLINED) 
					? TT_SoftStyle_Underlined : TT_SoftStyle_None,
				TAG_END);

#ifdef USE_SEMA
			ScalosReleaseSemaphore(&tteSema);
#endif
			SetFamilyTTFont(rp, style & enable, ttff);
			}
		}

	d1(kprintf("%s/%s/%ld: TxBaseline=%ld  tf_Baseline=%ld\n", __FILE__, __FUNC__, __LINE__, rp->TxBaseline, rp->Font->tf_Baseline));

	return SetSoftStyle(rp, style, enable);
}


SAVEDS(void) Scalos_SetFont(struct RastPort *rp, struct TextFont *tf, struct TTFontFamily *ttff)
{
	d1(kprintf("%s/%s/%ld: TTEngineBase=%08lx  ttFont=%08lx\n", __FILE__, __FUNC__, __LINE__, TTEngineBase, ttff));

	if (tf && (rp->Font != tf))
		SetFont(rp, tf);

	if (TTEngineBase && ttff && (ttff->ttff_BoldItalic || ttff->ttff_Bold || ttff->ttff_Italic || ttff->ttff_Normal))
		{
		SetFamilyTTFont(rp, FS_NORMAL, ttff);

#ifdef USE_SEMA
		ScalosObtainSemaphore(&tteSema);
#endif
		// TT_SetAttrsA()
		TT_SetAttrs(rp,
			TT_Antialias, CurrentPrefs.pref_TTFontAntialias,
			TT_Gamma, CurrentPrefs.pref_TTFontGamma,
			TT_ColorMap, (ULONG) iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap, 
//			TT_Screen, iInfos.xii_iinfos.ii_Screen,
			TAG_END);
#ifdef USE_SEMA
		ScalosReleaseSemaphore(&tteSema);
#endif
		}

	if (NULL == ttff || (NULL == ttff->ttff_BoldItalic && NULL == ttff->ttff_Bold &&
			NULL == ttff->ttff_Italic && NULL == ttff->ttff_Normal))
		Scalos_DoneRastPort(rp);	// remove TT font
}


static void SetFamilyTTFont(struct RastPort *rp, ULONG style, struct TTFontFamily *ttff)
{
	BOOL isBold = style & FSF_BOLD;
	BOOL isItalic = style & FSF_ITALIC;
	APTR ttFont;

	if (isBold && isItalic)
		{
		d1(kprintf("%s/%s/%ld: rp=%08lx BoldItalic font=%08lx\n", __FILE__, __FUNC__, __LINE__, rp, ttff->ttff_BoldItalic));
		ttFont = ttff->ttff_BoldItalic;
		}
	else if (isBold)
		{
		d1(kprintf("%s/%s/%ld: rp=%08lx Bold font=%08lx\n", __FILE__, __FUNC__, __LINE__, rp, ttff->ttff_Bold));
		ttFont = ttff->ttff_Bold;
		}
	else if (isItalic)
		{
		d1(kprintf("%s/%s/%ld: rp=%08lx Italic font=%08lx\n", __FILE__, __FUNC__, __LINE__, rp, ttff->ttff_Italic));
		ttFont = ttff->ttff_Italic;
		}
	else
		{
		d1(kprintf("%s/%s/%ld: rp=%08lx Normal font=%08lx\n", __FILE__, __FUNC__, __LINE__, rp, ttff->ttff_Normal));
		ttFont = ttff->ttff_Normal;
		}

	if (NULL == ttFont)
		{
		d1(kprintf("%s/%s/%ld: rp=%08lx Fallback to Normal font=%08lx\n", __FILE__, __FUNC__, __LINE__, rp, ttff->ttff_Normal));
		ttFont = ttff->ttff_Normal;
		}

	if (ttFont)
		{
#ifdef USE_SEMA
		ScalosObtainSemaphore(&tteSema);
#endif
		TT_SetFont(rp, ttFont);
		d1(kprintf("%s/%s/%ld: rp=%08lx ttFont=%08lx\n", __FILE__, __FUNC__, __LINE__, rp, ttFont));

		//TT_SetAttrsA()
		TT_SetAttrs(rp,
			TT_Antialias, CurrentPrefs.pref_TTFontAntialias,
			TT_Gamma, CurrentPrefs.pref_TTFontGamma,
			TT_ColorMap, (ULONG) iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap,
			TAG_END);
#ifdef USE_SEMA
		ScalosReleaseSemaphore(&tteSema);
#endif
		}
}


APTR Scalos_OpenTTFont(CONST_STRPTR FontDesc, struct TTFontFamily *ttff)
{
	d1(kprintf("%s/%s/%ld: FontDesc=<%s>\n", __FILE__, __FUNC__, __LINE__, FontDesc));
	memset(ttff, 0, sizeof(struct TTFontFamily));

	if (TTEngineBase && FontDesc)
		{
		struct TTTextAttr ttta;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (FillTTTextAttrFromDesc(&ttta, FontDesc))
			{
			d1(kprintf("%s/%s/%ld: FamilyName=<%s>  Size=%ld\n", \
				__FILE__, __FUNC__, __LINE__, ttta.ttta_FamilyTable[0], ttta.ttta_FontSize));

#ifdef USE_SEMA
			ScalosObtainSemaphore(&tteSema);
#endif

			// TT_OpenFontA()
			ttff->ttff_Normal = TT_OpenFont(
				TT_FamilyTable, (ULONG) ttta.ttta_FamilyTable,
				TT_FontSize, ttta.ttta_FontSize,
				TT_FontStyle, ttta.ttta_FontStyle,
				TT_FontWeight, ttta.ttta_FontWeight,
				TAG_END);
			d1(kprintf("%s/%s/%ld: Normal font=%08lx\n", __FILE__, __FUNC__, __LINE__, ttff->ttff_Normal));

			ttff->ttff_Bold = TT_OpenFont(
				TT_FamilyTable, (ULONG) ttta.ttta_FamilyTable,
				TT_FontSize, ttta.ttta_FontSize,
				TT_FontStyle, TT_FontStyle_Regular,
				TT_FontWeight, TT_FontWeight_Bold,
				TAG_END);
			d1(kprintf("%s/%s/%ld: Bold font=%08lx\n", __FILE__, __FUNC__, __LINE__, ttff->ttff_Bold));

			ttff->ttff_Italic = TT_OpenFont(
				TT_FamilyTable, (ULONG) ttta.ttta_FamilyTable,
				TT_FontSize, ttta.ttta_FontSize,
				TT_FontStyle, TT_FontStyle_Italic,
				TT_FontWeight, TT_FontWeight_Normal,
				TAG_END);
			d1(kprintf("%s/%s/%ld: Italic font=%08lx\n", __FILE__, __FUNC__, __LINE__, ttff->ttff_Italic));

			ttff->ttff_BoldItalic = TT_OpenFont(
				TT_FamilyTable, (ULONG) ttta.ttta_FamilyTable,
				TT_FontSize, ttta.ttta_FontSize,
				TT_FontStyle, TT_FontStyle_Italic,
				TT_FontWeight, TT_FontWeight_Bold,
				TAG_END);
			d1(kprintf("%s/%s/%ld: BoldItalic font=%08lx\n", __FILE__, __FUNC__, __LINE__, ttff->ttff_BoldItalic));

#ifdef USE_SEMA
			ScalosReleaseSemaphore(&tteSema);
#endif
			}
		}

	return ttff->ttff_Normal;
}


void Scalos_CloseFont(struct TextFont **tf, struct TTFontFamily *ttFont)
{
	if (TTEngineBase && ttFont)
		{
		d1(kprintf("%s/%s/%ld: Normal font=%08lx\n", __FILE__, __FUNC__, __LINE__, ttFont->ttff_Normal));

#ifdef USE_SEMA
		ScalosObtainSemaphore(&tteSema);
#endif
		TT_CloseFont(ttFont->ttff_Normal);
		TT_CloseFont(ttFont->ttff_Bold);
		TT_CloseFont(ttFont->ttff_Italic);
		TT_CloseFont(ttFont->ttff_BoldItalic);

#ifdef USE_SEMA
		ScalosReleaseSemaphore(&tteSema);
#endif
		ttFont->ttff_Normal = NULL;
		ttFont->ttff_Bold = NULL;
		ttFont->ttff_Italic = NULL;
		ttFont->ttff_BoldItalic = NULL;
		}

	if (tf && *tf)
		{
		CloseFont(*tf);
		*tf = NULL;
		}
}


SAVEDS(void) Scalos_InitRastPort(struct RastPort *rp)
{
	InitRastPort(rp);

	if (TTEngineBase)
		{
#ifdef USE_SEMA
		ScalosObtainSemaphore(&tteSema);
#endif
		TT_SetAttrs(rp,
			TT_ColorMap, (ULONG) iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap, 
			TAG_END);

#ifdef USE_SEMA
		ScalosReleaseSemaphore(&tteSema);
#endif
		d1(kprintf("%s/%s/%ld: rp=%08lx  cm=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, rp, iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap));
		}
}


SAVEDS(void) Scalos_DoneRastPort(struct RastPort *rp)
{
	d1(kprintf("%s/%s/%ld: rp=%08lx\n", __FILE__, __FUNC__, __LINE__, rp));

	if (TTEngineBase)
		{
#ifdef USE_SEMA
		ScalosObtainSemaphore(&tteSema);
#endif
		TT_DoneRastPort(rp);
#ifdef USE_SEMA
		ScalosReleaseSemaphore(&tteSema);
#endif
		}
}


static BOOL FillTTTextAttrFromDesc(struct TTTextAttr *ttta, CONST_STRPTR FontDesc)
{
	// Font Desc format:
	// "style/weight/size/fontname"
	char Buffer[10];
	size_t n;

	ttta->ttta_FontStyle = TT_FontStyle_Regular;
	ttta->ttta_FontWeight = TT_FontWeight_Normal;
	
	d1(kprintf("%s/%s/%ld: ttta=%08lx  FontDesc=<%s>\n", __FILE__, __FUNC__, __LINE__, ttta, FontDesc));

	for (n = 0; *FontDesc && '/' != *FontDesc && n < sizeof(Buffer) - 1; n++)
		Buffer[n] = *FontDesc++;
	Buffer[n] = '\0';
	d1(kprintf("%s/%s/%ld: Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, Buffer));

	if (StrToLong(Buffer, (LONG *) &ttta->ttta_FontStyle) <= 0)
		return FALSE;

	if ('/' == *FontDesc)
		FontDesc++;	// Skip "/" between style and weight
	else
		return FALSE;

	for (n = 0; *FontDesc && '/' != *FontDesc && n < sizeof(Buffer) - 1; n++)
		Buffer[n] = *FontDesc++;
	Buffer[n] = '\0';
	d1(kprintf("%s/%s/%ld: Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, Buffer));

	if (StrToLong(Buffer, (LONG *) &ttta->ttta_FontWeight) <= 0)
		return FALSE;

	if ('/' == *FontDesc)
		FontDesc++;	// Skip "/" between weight and size
	else
		return FALSE;

	for (n = 0; *FontDesc && '/' != *FontDesc && n < sizeof(Buffer) - 1; n++)
		Buffer[n] = *FontDesc++;
	Buffer[n] = '\0';
	d1(kprintf("%s/%s/%ld: Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, Buffer));

	if (StrToLong(Buffer, (LONG *) &ttta->ttta_FontSize) <= 0)
		return FALSE;

	d1(kprintf("%s/%s/%ld: ttta=%08lx\n", __FILE__, __FUNC__, __LINE__, ttta));

	if ('/' == *FontDesc)
		FontDesc++;	// Skip "/" between size and name
	else
		return FALSE;

	d1(kprintf("%s/%s/%ld: FontDesc=<%s>\n", __FILE__, __FUNC__, __LINE__, FontDesc));

	// copy font name
	stccpy(ttta->ttta_NameSpace, FontDesc, sizeof(ttta->ttta_NameSpace));
	ttta->ttta_FamilyTable[0] = ttta->ttta_NameSpace;
	ttta->ttta_FamilyTable[1] = NULL;

	return TRUE;		// Success!
}


static SAVEDS(ULONG) FontFunctionDispatcher(struct Hook *hook, Object *o, Msg msg)
{
	ULONG Result = 0;

	d1(kprintf("%s/%s/%ld: hook=%08lx  o=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, hook, o, msg));
	d1(kprintf("%s/%s/%ld: MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->MethodID));

	switch (msg->MethodID)
		{
	case iofMsg_DONERASTPORT:
		{
		struct ioFontMsg_DoneRastPort *iofdr = (struct ioFontMsg_DoneRastPort *) msg;

		Scalos_DoneRastPort(iofdr->iofdr_RastPort);
		}
		break;
	case iofMsg_SETFONT:
		{
		struct ioFontMsg_SetFont *iofsf = (struct ioFontMsg_SetFont *) msg;

		Scalos_SetFont(iofsf->iofsf_RastPort, iofsf->iofsf_TextFont, iofsf->iofsf_FontHandle);
		}
		break;
	case iofMsg_TEXT:
		{
		struct ioFontMsg_Text *ioft = (struct ioFontMsg_Text *) msg;

		Scalos_Text(ioft->ioft_RastPort, ioft->ioft_String, ioft->ioft_Length);
		}
		break;
	case iofMsg_SETFONTSTYLE:
		{
		struct ioFontMsg_SetFontStyle *iofsfs = (struct ioFontMsg_SetFontStyle *) msg;

		Result = Scalos_SetSoftStyle(iofsfs->iofsfs_RastPort, 
			iofsfs->iofsfs_Style, iofsfs->iofsfs_Enable, 
			iofsfs->iofsfs_FontHandle);
		}
		break;
	case iofMsg_TEXTEXTENT:
		{
		struct ioFontMsg_TextExtent *iofte = (struct ioFontMsg_TextExtent *) msg;

		d1(kprintf("%s/%s/%ld: rp=%08lx  string=%08lx  length=%lu\n", \
			__FILE__, __FUNC__, __LINE__, iofte->iofte_RastPort, iofte->iofte_String, iofte->iofte_Length));

		Scalos_TextExtent(iofte->iofte_RastPort, iofte->iofte_String, 
			iofte->iofte_Length, iofte->iofte_TextExtent);
		}
		break;
	case iofMsg_TEXTFIT:
		{
		struct ioFontMsg_TextFit *ioftf = (struct ioFontMsg_TextFit *) msg;

		Result = Scalos_TextFit(ioftf->ioftf_RastPort, ioftf->ioftf_String, 
			ioftf->ioftf_Length, ioftf->ioftf_TextExtent,
			ioftf->ioftf_ConstrainingTextExtent, ioftf->ioftf_StrDirection,
			ioftf->ioftf_ConstrainingBitWidth, ioftf->ioftf_ConstrainingBitHeight);
		}
		break;
	case iofMsg_GETFONTHEIGHT:
		{
		struct ioFontMsg_GetFontHeight *iofgfh = (struct ioFontMsg_GetFontHeight *) msg;

		Result = Scalos_GetFontHeight(iofgfh->iofgfh_RastPort);
		}
		break;
	case iofMsg_GETFONTBASELINE:
		{
		struct ioFontMsg_GetFontBaseline *iofgfb = (struct ioFontMsg_GetFontBaseline *) msg;

		Result = Scalos_GetFontBaseline(iofgfb->iofgfb_RastPort);
		}
		break;
	case iofMsg_SETTRANSPARENCY:
		{
		struct ioFontMsg_SetTransparency *iofst = (struct ioFontMsg_SetTransparency *) msg;

		Scalos_SetFontTransparency(iofst->iofst_RastPort, iofst->iofst_Transparency);
		}
		break;
	default:
		break;
		}

	d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}

