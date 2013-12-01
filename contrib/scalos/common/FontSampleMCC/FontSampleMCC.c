// FontSampleMCC.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/mui.h>
#include <libraries/ttengine.h>
#include <graphics/text.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#define USE_INLINE_STDARG
#include <proto/ttengine.h>
#undef	USE_INLINE_STDARG
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include "FontSampleMCC.h"

/* ------------------------------------------------------------------------- */

struct FsMCCInstance
	{
	APTR	fsm_TTFontHandle;	// Font handle from TT_OpenFontA()
	struct TextFont *fsm_StdFont;	// Font handle from OpenDiskFont()
	STRPTR fsm_String;		// Demo text that displays in area
	STRPTR fsm_TTFontDesc;		// TT Font descriptor
	STRPTR fsm_StdFontDesc;		// Standard Font descriptor
	ULONG fsm_FontBaseLine;
	BYTE fsm_AntiAlias;
	WORD fsm_Gamma;
	BOOL fsm_NeedRelayout;
	ULONG fsm_HAlign;		// horizontal alignment
	ULONG fsm_VAlign;		// vertical  alignment
	struct TextExtent fsm_TextExtent;
	};

/* ------------------------------------------------------------------------- */

extern struct Library *TTEngineBase;

/* ------------------------------------------------------------------------- */

static ULONG mNew(struct IClass *cl,Object *obj,Msg msg);
static ULONG mDispose(struct IClass *cl, Object *obj, Msg msg);
static ULONG mAskMinMax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg);
static ULONG mDraw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg);
static ULONG mSet(struct IClass *cl, Object *obj, struct opSet *ops);
DISPATCHER_PROTO(FontSampleMCC);
static void ClearInstanceData(struct FsMCCInstance *inst);
static BOOL SetInstanceData(struct FsMCCInstance *inst, struct TagItem *TagList);
static APTR OpenTTFontFromDesc(CONST_STRPTR FontDesc);
static struct TextFont *OpenDiskFontFromDesc(CONST_STRPTR FontDesc);
static void ForceRelayout(struct IClass *cl, Object *obj);
#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
static size_t stccpy(char *dest, const char *src, size_t MaxLen);
#endif /* !__SASC && !__MORPHOS__ && !__amigaos4__ */

/* ------------------------------------------------------------------------- */

static ULONG mNew(struct IClass *cl,Object *obj,Msg msg)
{
	d1(KPrintF("%s/%ld: START  obj=%08lx\n", __FUNC__, __LINE__, obj));

	do	{
		struct FsMCCInstance *inst;
		struct opSet *ops;

		d1(KPrintF("%s/%ld: cl=%08lx  obj=%08lx  msg=%08lx\n", __FUNC__, __LINE__, cl, obj, msg));

		obj = (Object *) DoSuperMethodA(cl, obj, msg);
		d1(KPrintF("%s/%ld: obj=%08lx\n", __FUNC__, __LINE__, obj));
		if (NULL == obj)
			break;

		inst = INST_DATA(cl,obj);
		ClearInstanceData(inst);

		inst->fsm_String = strdup("The quick brown fox jumps over the lazy dog");

		set(obj, MUIA_FillArea, TRUE);

		ops = (struct opSet *) msg;

		SetInstanceData(inst, ops->ops_AttrList);

		inst->fsm_NeedRelayout = FALSE;
		} while (0);

	d1(KPrintF("%s/%ld: END  obj=%08lx\n", __FUNC__, __LINE__, obj));

	return((ULONG)obj);
}


static ULONG mDispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct FsMCCInstance *inst = INST_DATA(cl,obj);

	if (TTEngineBase && inst->fsm_TTFontHandle)
		{
		TT_CloseFont(inst->fsm_TTFontHandle);
		inst->fsm_TTFontHandle = NULL;
		}
	if (inst->fsm_StdFont)
		{
		CloseFont(inst->fsm_StdFont);
		inst->fsm_StdFont = NULL;
		}
	if (inst->fsm_TTFontDesc)
		{
		free(inst->fsm_TTFontDesc);
		inst->fsm_TTFontDesc = NULL;
		}
	if (inst->fsm_StdFontDesc)
		{
		free(inst->fsm_StdFontDesc);
		inst->fsm_StdFontDesc = NULL;
		}
	if (inst->fsm_String)
		{
		free(inst->fsm_String);
		inst->fsm_String = NULL;
		}

	return DoSuperMethodA(cl, obj, msg);
}


static ULONG mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	struct FsMCCInstance *inst = INST_DATA(cl,obj);
	struct MUI_MinMax *mi;

	DoSuperMethodA(cl, obj, (Msg) msg);

	mi = msg->MinMaxInfo;

	mi->MinWidth  += 100;
	mi->MinHeight += 4 + inst->fsm_TextExtent.te_Height;
	mi->DefWidth  += 120;
	mi->DefHeight += 4 + inst->fsm_TextExtent.te_Height;

	mi->MaxWidth  = MUI_MAXMAX;
	mi->MaxHeight = MUI_MAXMAX;

	/* if we cannot show TT text, our minimal size will be 0 */

	return(0);
}


static ULONG mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct FsMCCInstance *inst = INST_DATA(cl,obj);

	DoSuperMethodA(cl, obj, (Msg) msg);

	if (msg->flags & MADF_DRAWOBJECT)
		{
		APTR ClipHandle;
		LONG x, y;

		d1(kprintf("%s/%s/%ld: TTEngineBase=%08lx  fsm_TTFontHandle=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, TTEngineBase, inst->fsm_TTFontHandle));
		d1(kprintf("%s/%s/%ld: width=%ld  height=%ld\n", __FILE__, __FUNC__, __LINE__, _mwidth(obj), _mheight(obj)));
		d1(kprintf("%s/%s/%ld: subwidth=%ld  subheight=%ld\n", __FILE__, __FUNC__, __LINE__, _subwidth(obj), _subheight(obj)));

		ClipHandle = MUI_AddClipping(muiRenderInfo(obj), _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj));

		switch (inst->fsm_HAlign)
			{
		default:
		case FONTSAMPLE_HALIGN_LEFT:
			x = _mleft(obj) + 10;
			break;
		case FONTSAMPLE_HALIGN_CENTER:
			x = _mleft(obj) + (_mwidth(obj) - inst->fsm_TextExtent.te_Width) / 2;
			break;
		case FONTSAMPLE_HALIGN_RIGHT:
			x = _mright(obj) - 10 - inst->fsm_TextExtent.te_Width;
			break;
			}

		switch (inst->fsm_VAlign)
			{
		default:
		case FONTSAMPLE_VALIGN_TOP:
			y = _mtop(obj)  + 2 + inst->fsm_FontBaseLine;
			break;
		case FONTSAMPLE_VALIGN_CENTER:
			y = _mtop(obj)  + (_mheight(obj) - inst->fsm_TextExtent.te_Height) / 2 + inst->fsm_FontBaseLine;
			break;
		case FONTSAMPLE_VALIGN_BOTTOM:
			y = _mbottom(obj) - 2 - inst->fsm_TextExtent.te_Height + inst->fsm_FontBaseLine;
			break;
			}

		Move(_rp(obj), x, y);
		SetAPen(_rp(obj), _pens(obj)[MPEN_TEXT]);

		if (TTEngineBase && inst->fsm_TTFontHandle)
			{
			TT_SetFont(_rp(obj), inst->fsm_TTFontHandle);

			TT_SetAttrs(_rp(obj), 
				TT_Window, (ULONG) _window(obj),
				TT_Antialias, inst->fsm_AntiAlias,
				TT_Gamma, inst->fsm_Gamma,
				TAG_END);

			TT_Text(_rp(obj), (STRPTR) inst->fsm_String, strlen(inst->fsm_String));

			TT_DoneRastPort(_rp(obj));
			}
		else
			{
			 if (inst->fsm_StdFont)
				SetFont(_rp(obj), inst->fsm_StdFont);

			SetSoftStyle(_rp(obj), FS_NORMAL, FSF_BOLD | FSF_ITALIC | FSF_UNDERLINED | FSF_EXTENDED);

			Text(_rp(obj), (STRPTR) inst->fsm_String, strlen(inst->fsm_String));
			}

		MUI_RemoveClipping(muiRenderInfo(obj), ClipHandle);
		}

	return(0);
}


static ULONG mSet(struct IClass *cl,Object *obj, struct opSet *ops)
{
	struct FsMCCInstance *inst = INST_DATA(cl,obj);
	BOOL NeedRedraw;

	DoSuperMethodA(cl, obj, (Msg) ops);

	NeedRedraw = SetInstanceData(inst, ops->ops_AttrList);

	if (inst->fsm_NeedRelayout)
		ForceRelayout(cl, obj);

	if (NeedRedraw)
		MUI_Redraw(obj, MADF_DRAWOBJECT);

	return 1;
}


DISPATCHER(FontSampleMCC)
{
	ULONG Result;

	d1(KPrintF("%s/%ld: START  MethodID=%08lx\n", __FUNC__, __LINE__, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:
		d1(kprintf("%s/%s/%ld: OM_NEW\n", __FILE__, __FUNC__, __LINE__));
		Result = mNew(cl, obj, (APTR)msg);
		break;

	case OM_DISPOSE:
		d1(kprintf("%s/%s/%ld: OM_DISPOSE\n",  __FILE__, __FUNC__, __LINE__));
		Result = mDispose(cl,obj,(APTR)msg);
		break;

	case OM_SET:
		d1(kprintf("%s/%s/%ld: OM_SET\n",  __FILE__, __FUNC__, __LINE__));
		Result = mSet(cl, obj, (struct opSet *) msg);
		break;

	case MUIM_AskMinMax:
		d1(kprintf("%s/%s/%ld: MUIM_AskMinMax\n",  __FILE__, __FUNC__, __LINE__));
		Result = mAskMinMax(cl,obj,(APTR)msg);
		break;

	case MUIM_Draw:
		d1(kprintf("%s/%s/%ld: MUIM_Draw\n",  __FILE__, __FUNC__, __LINE__));
		Result = mDraw(cl,obj,(APTR)msg);
		break;

	default:
		d1(kprintf("%s/%s/%ld: MethodID=%08lx\n",  __FILE__, __FUNC__, __LINE__, msg->MethodID));
		Result = DoSuperMethodA(cl,obj,msg);
		break;
		}

	d1(KPrintF("%s/%ld: END  Result=%08lx\n", __FUNC__, __LINE__, Result));

	return Result;
}
DISPATCHER_END


static void ClearInstanceData(struct FsMCCInstance *inst)
{
	memset(inst, 0, sizeof(struct FsMCCInstance));

	inst->fsm_AntiAlias = TT_Antialias_On;
	inst->fsm_Gamma = 2500;
	inst->fsm_HAlign = FONTSAMPLE_HALIGN_CENTER;
	inst->fsm_VAlign = FONTSAMPLE_VALIGN_CENTER;
}


static BOOL SetInstanceData(struct FsMCCInstance *inst, struct TagItem *TagList)
{
	struct TagItem *tag;
	BOOL Redraw = FALSE;
	BYTE OldAntiAlias = inst->fsm_AntiAlias;
	WORD OldGamma = inst->fsm_Gamma;

	inst->fsm_AntiAlias = (BYTE) GetTagData(MUIA_FontSample_Antialias, (ULONG) inst->fsm_AntiAlias, TagList);
	inst->fsm_Gamma = (WORD) GetTagData(MUIA_FontSample_Gamma, (ULONG) inst->fsm_Gamma, TagList);
	inst->fsm_HAlign = (WORD) GetTagData(MUIA_FontSample_HAlign, (ULONG) inst->fsm_HAlign, TagList);
	inst->fsm_VAlign = (WORD) GetTagData(MUIA_FontSample_VAlign, (ULONG) inst->fsm_VAlign, TagList);

	tag = FindTagItem(MUIA_FontSample_DemoString, TagList);
	if (tag)
		{
		if (inst->fsm_String)
			free(inst->fsm_String);
		inst->fsm_String = strdup((STRPTR) tag->ti_Data);
		}

	tag = FindTagItem(MUIA_FontSample_TTFontDesc, TagList);
	if (tag)
		{
		if (inst->fsm_TTFontDesc)
			free(inst->fsm_TTFontDesc);
		inst->fsm_TTFontDesc = strdup((STRPTR) tag->ti_Data);
		}

	tag = FindTagItem(MUIA_FontSample_StdFontDesc, TagList);
	if (tag)
		{
		if (inst->fsm_StdFontDesc)
			free(inst->fsm_StdFontDesc);
		inst->fsm_StdFontDesc = strdup((STRPTR) tag->ti_Data);
		}

	if (TTEngineBase && inst->fsm_TTFontDesc)
		{
		if (inst->fsm_TTFontHandle)
			TT_CloseFont(inst->fsm_TTFontHandle);

		inst->fsm_TTFontHandle = OpenTTFontFromDesc(inst->fsm_TTFontDesc);

		d1(kprintf("%s/%s/%ld: fsm_TTFontHandle = %08lx FontDesc=<%s>\n", \
			_ __FILE__, __FUNC__, _LINE__, inst->fsm_TTFontHandle, inst->fsm_TTFontDesc ? inst->fsm_TTFontDesc : (STRPTR) ""));

		// Force redraw if antialias or gamma has changed
		if (OldGamma != inst->fsm_Gamma || OldAntiAlias != inst->fsm_AntiAlias)
			Redraw = TRUE;

		if (inst->fsm_TTFontHandle)
			{
			struct RastPort rp;
			WORD OldHeight;


			OldHeight = inst->fsm_TextExtent.te_Height;

			InitRastPort(&rp);

			TT_SetFont(&rp, inst->fsm_TTFontHandle);

			TT_GetAttrs(&rp, TT_FontAccentedAscender, (ULONG) &inst->fsm_FontBaseLine,
				TAG_END);

			TT_TextExtent(&rp, (STRPTR) inst->fsm_String, strlen(inst->fsm_String), &inst->fsm_TextExtent);

			d1(kprintf("%s/%s/%ld: fsm_TextExtent width=%ld  height=%ld\n", \
				 __FILE__, __FUNC__, __LINE__, inst->fsm_TextExtent.te_Width, inst->fsm_TextExtent.te_Height));

			TT_DoneRastPort(&rp);

			Redraw = TRUE;

			if (OldHeight != inst->fsm_TextExtent.te_Height)
				inst->fsm_NeedRelayout = TRUE;
			}
		}
	else if (inst->fsm_StdFontDesc)
		{
		struct RastPort rp;
		WORD OldHeight;

		if (inst->fsm_StdFont)
			CloseFont(inst->fsm_StdFont);

		inst->fsm_StdFont = OpenDiskFontFromDesc(inst->fsm_StdFontDesc);

		d1(kprintf("%s/%s/%ld: fsm_StdFont=%08lx FontDesc=<%s>\n", \
			 __FILE__, __FUNC__, __LINE__, inst->fsm_StdFont, inst->fsm_StdFontDesc ? inst->fsm_StdFontDesc : (STRPTR) ""));

		OldHeight = inst->fsm_TextExtent.te_Height;

		InitRastPort(&rp);

		if (inst->fsm_StdFont)
			{
			SetFont(&rp, inst->fsm_StdFont);

			inst->fsm_FontBaseLine = inst->fsm_StdFont->tf_Baseline;
			}

		TextExtent(&rp, (STRPTR) inst->fsm_String, strlen(inst->fsm_String), &inst->fsm_TextExtent);

		d1(kprintf("%s/%s/%ld: fsm_TextExtent width=%ld  height=%ld\n", \
			 __FILE__, __FUNC__, __LINE__, inst->fsm_TextExtent.te_Width, inst->fsm_TextExtent.te_Height));

		Redraw = TRUE;

		if (OldHeight != inst->fsm_TextExtent.te_Height)
			inst->fsm_NeedRelayout = TRUE;
		}

	return Redraw;
}


static APTR OpenTTFontFromDesc(CONST_STRPTR FontDesc)
{
	ULONG FontStyle;
	ULONG FontWeight;
	ULONG FontSize;
	CONST_STRPTR lp;
	char NameSpace[MAX_TTFONTDESC];
	STRPTR FamilyTable[2];
	APTR TTFont;
	long long1, long2, long3;

	// Font Desc format:
	// "style/weight/size/fontname"

	if (NULL == FontDesc)
		return NULL;

	if (3 != sscanf(FontDesc, "%ld/%ld/%ld", &long1, &long2, &long3))
		return NULL;

	FontStyle = long1;
	FontWeight = long2;
	FontSize = long3;

	lp = strchr(FontDesc, '/');	// Find "/" between style and weight
	if (NULL == lp)
		return NULL;

	lp = strchr(lp + 1, '/');	// Find "/" between weight and size
	if (NULL == lp)
		return NULL;

	lp = strchr(lp + 1, '/');	// Find "/" between size and name
	if (NULL == lp)
		return NULL;

	// copy font name
	stccpy(NameSpace, 1 + lp, sizeof(NameSpace));
	FamilyTable[0] = NameSpace;
	FamilyTable[1] = NULL;

	//TT_OpenFontA()
	TTFont = TT_OpenFont(TT_FamilyTable, (ULONG) FamilyTable,
		TT_FontSize, FontSize,
		TT_FontStyle, FontStyle,
		TT_FontWeight, FontWeight,
		TAG_END);

	return TTFont;
}


static struct TextFont *OpenDiskFontFromDesc(CONST_STRPTR FontDesc)
{
	// Font Desc format:
	// "fontname/size"
	struct TextFont *Font;
	struct TextAttr ta;
	CONST_STRPTR fp;
	size_t fnLen;
	STRPTR FontName;
	LONG l;

	for (fp=FontDesc, fnLen=0; *fp && '/' != *fp; fnLen++)
		fp++;

	ta.ta_Name = FontName = malloc(fnLen + 5 + 1);
	if (NULL == FontName)
		return NULL;

	stccpy(ta.ta_Name, FontDesc, 1 + fnLen);
	if ((strlen(ta.ta_Name) < strlen(".font")) ||
		(0 != stricmp(ta.ta_Name + strlen(ta.ta_Name) - strlen(".font"), ".font")))
		{
		strcat(ta.ta_Name, ".font");
		}

	if ('/' == *fp)
		fp++;

	if (StrToLong(fp, &l))
		ta.ta_YSize = l;
	else
		ta.ta_YSize = 8;

	ta.ta_Style = FS_NORMAL;
	ta.ta_Flags = 0;

	Font = OpenDiskFont(&ta);

	d1(kprintf("%s/%s/%ld: Font=%08lx  FontName=<%s>  Size=%ld\n",  __FILE__, __FUNC__, __LINE__, Font, FontName, ta.ta_YSize));

	free(FontName);

	return Font;
}


static void ForceRelayout(struct IClass *cl, Object *obj)
{
	struct FsMCCInstance *inst = INST_DATA(cl,obj);
	Object *WindowObj;
	Object *RootObj = NULL;

	inst->fsm_NeedRelayout = FALSE;	

	if (muiRenderInfo(obj))
		{
		WindowObj = _win(obj);
		get(WindowObj, MUIA_Window_RootObject, &RootObj);

		if (RootObj)
			{
			// force relayout
			DoMethod(RootObj, MUIM_Group_InitChange);
			DoMethod(RootObj, MUIM_Group_ExitChange);
			}
		}
}

/* ------------------------------------------------------------------------- */

struct MUI_CustomClass *InitFontSampleClass(void)
{
	return MUI_CreateCustomClass(NULL, MUIC_Area, NULL, sizeof(struct FsMCCInstance), DISPATCHER_REF(FontSampleMCC));
}

void CleanupFontSampleClass(struct MUI_CustomClass *mcc)
{
	MUI_DeleteCustomClass(mcc);
}

/* ------------------------------------------------------------------------- */

#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
// Replacement for SAS/C library functions

static size_t stccpy(char *dest, const char *src, size_t MaxLen)
{
	size_t Count = 0;

	while (*src && MaxLen > 1)
		{
		*dest++ = *src++;
		MaxLen--;
		Count++;
		}
	*dest = '\0';
	Count++;

	return Count;
}

#endif /* !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) */

