/***************************************************************************

 NListtree.mcp - New Listtree MUI Custom Class Preferences
 Copyright (C) 1999-2004 by Carsten Scholling <aphaso@aphaso.de>,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

/*
**	Includes
*/
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/locale.h>
#include <proto/exec.h>

#ifdef __AROS__
#define MUI_OBSOLETE
#endif
#include <proto/muimaster.h>

#include <utility/hooks.h>
#include <exec/types.h>

#include "locale.h"

#include <MUI/NFloattext_mcc.h>
#include <MUI/NListview_mcc.h>
#include <MUI/NList_mcc.h>

#include "rev.h"
#include "private.h"

#include <clib/alib_protos.h>

#define MSG_DRAW2		  MSG_DUMMY
#define MSG_DRAW2_KEY	MSG_DUMMY

#if defined(__amigaos4__) || defined(__MORPHOS__)
struct Library *LocaleBase;
#else
struct LocaleBase *LocaleBase;
#endif

#if defined(__amigaos4__)
struct LocaleIFace *ILocale;
#endif
struct Locale *Locale;

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

// some undocumented MUI tags we are going to use
#define MUIA_Imagedisplay_UseDefSize  0x8042186d /* V11 i.. BOOL */

// add a replacemnet define for the standard sprintf
#define sprintf MySPrintf

#ifdef __AROS__
AROS_UFH2S(void, putchfunc, 
    AROS_UFHA(UBYTE, chr, D0),
    AROS_UFHA(UBYTE **, data, A3))
{
    AROS_USERFUNC_INIT
    
    *(*data)++ = chr;
    
    AROS_USERFUNC_EXIT
}

int MySPrintf(char *buf, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

#if 0
	APTR raw_args[] = { &args, fmt };
	RawDoFmt("%V", raw_args, NULL, buf);
#else
#warning "FIXME: Code above is better, but doesn't work because locale.library RawDoFmt"
#warning "       replacement doesn't know %V"

    	{
    	    UBYTE *bufptr = buf;
	
	    RawDoFmt(fmt, &fmt + 1, NULL, bufptr);
    	}
#endif
	
	va_end(args);
	
	return strlen(buf);
}
#elif defined(__amigaos4__)
int VARARGS68K MySPrintf(char *buf, char *fmt, ...)
{
	va_list args;
	va_startlinear(args, fmt);

	RawDoFmt(fmt, va_getlinearva(args, void *), NULL, buf);

	va_end(args);
	return(strlen(buf));
}
#elif defined(__MORPHOS__)
int VARARGS68K MySPrintf(char *buf, char *fmt,...)
{
	va_list args;
	va_start(args, fmt);

	RawDoFmt(fmt, args->overflow_arg_area, NULL, buf);

	va_end(args);
	return(strlen(buf));
}
#else
int STDARGS MySPrintf(char *buf, char *fmt,...)
{
	static const UWORD PutCharProc[2] = {0x16C0,0x4E75};
	/* dirty hack to avoid assembler part :-)
   	16C0: move.b d0,(a3)+
	   4E75: rts */
	RawDoFmt(fmt, (APTR)(((ULONG)&fmt)+4), (APTR)PutCharProc, buf);

	return(strlen(buf));
}
#endif

struct SampleArray
{
	char	*name;
	UWORD   flags;
};

/*
**	Sample
*/
static const struct SampleArray sa[] =
{
	{ "comp", TNF_LIST | TNF_OPEN           },
	{	  "sys", TNF_LIST | TNF_OPEN          },
	{	 		"amiga", TNF_LIST | TNF_OPEN      },
	{	 			"misc", 0x8000                  },
	{	 		"mac", TNF_LIST                   },
	{	 			"system", 0x8000                },

	{ "de", TNF_LIST | TNF_OPEN             },
	{	  "comm", TNF_LIST                    },
	{		  "software", TNF_LIST              },
	{		 		"ums", 0x8000                   },
	{	  "comp", TNF_LIST | TNF_OPEN         },
	{	 		"sys", TNF_LIST | TNF_OPEN        },
	{	 			"amiga", TNF_LIST               },
	{	 				"misc", 0x8000                },
	{	 				"tech", 0x8000                },
	{	 			"amiga", 0x8000                 },

	{ "m", TNF_LIST                         },
	{	 	"i", TNF_LIST                       },
	{ 		"c", TNF_LIST                     },
	{	 			"h", TNF_LIST                   },
	{	 				"e", TNF_LIST                 },
	{	 					"l", TNF_LIST               },
	{	 						"a", TNF_LIST             },
	{	 							"n", TNF_LIST           },
	{	 								"g", TNF_LIST         },
	{ 									"e", TNF_LIST       },
	{ 										"l", TNF_LIST     },
	{ 											"o", 0          },

	{ "end", TNF_LIST                       },
	{ 	"of", TNF_LIST                      },
	{ 		"tree", 0                         },

	{ NULL, 0 }
};



#define LSC(a,b) GetChar(a)

UBYTE GetChar( APTR CatStr )
{
	STRPTR Str;

	Str = GetString( CatStr );

	return( *Str );
}


/*****************************************************************************\
*******************************************************************************
**
**	Helpfull object related functions.
**
*******************************************************************************
\*****************************************************************************/

LONG xget( Object *obj, ULONG attribute )
{
	LONG x;

	get( obj, attribute, &x );
	return( x );
}

static VOID DrawSampleTree( Object *ltobj )
{
	struct MUI_NListtree_TreeNode *tn1, *tn2, *tn3;
	WORD i = 0;

	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;

	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn3 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn3, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn2 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn3 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn3 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn2, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn3 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;

	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;

	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
	tn1 = (struct MUI_NListtree_TreeNode *)DoMethod( ltobj, MUIM_NListtree_Insert, sa[i].name, &sa[i], tn1, MUIV_NListtree_Insert_PrevNode_Tail, sa[i].flags ); i++;
}


VOID TransferValues(struct NListtreeP_Data *data)
{
	LONG v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10;

	/*
	**	Style
	*/
	get( data->CY_Style,			MUIA_Cycle_Active,	&v0 );
	get( data->SL_Space,			MUIA_Slider_Level,	&v1 );
	get( data->CH_RememberStatus,	MUIA_Selected,		&v2 );
	get( data->CH_OpenAutoScroll,	MUIA_Selected,		&v3 );


	/*
	**	Images
	*/
	get( data->PI_ImageClosed,	MUIA_Imagedisplay_Spec,	&v4 );
	get( data->PI_ImageOpen,	MUIA_Imagedisplay_Spec,	&v5 );
	get( data->PI_ImageSpecial,	MUIA_Imagedisplay_Spec,	&v6 );


	/*
	**	Colors
	*/
	get( data->PP_LinePen,		MUIA_Pendisplay_Spec,	&v7 );
	get( data->PP_ShadowPen,	MUIA_Pendisplay_Spec,	&v8 );
	get( data->PP_DrawPen,		MUIA_Pendisplay_Spec,	&v9 );
	get( data->PP_Draw2Pen,		MUIA_Pendisplay_Spec,	&v10 );


	nnset( data->NLT_Sample, MUIA_NListtree_Quiet, TRUE );

	SetAttrs( data->NLT_Sample,	MUICFG_NListtree_Style,				v0,
								MUICFG_NListtree_Space, 			v1,
								MUICFG_NListtree_RememberStatus,	v2,
								MUICFG_NListtree_OpenAutoScroll,	v3,
								MUICFG_NListtree_ImageSpecClosed,	v4,
								MUICFG_NListtree_ImageSpecOpen,		v5,
								MUICFG_NListtree_ImageSpecSpecial,	v6,
								MUICFG_NListtree_PenSpecLines,		v7,
								MUICFG_NListtree_PenSpecShadow,		v8,
								MUICFG_NListtree_PenSpecDraw,		v9,
								MUICFG_NListtree_PenSpecDraw2,		v10,
		TAG_DONE );

	nnset( data->NLT_Sample, MUIA_NListtree_Quiet, FALSE );

	return;
}



/*****************************************************************************\
*******************************************************************************
**
**	Standard class related functions.
**
*******************************************************************************
\*****************************************************************************/

#ifdef __AROS__
AROS_HOOKPROTONHNO(dspfunc, LONG, struct MUIP_NListtree_DisplayMessage *, msg)
#else
HOOKPROTONHNO(dspfunc, LONG, struct MUIP_NListtree_DisplayMessage *msg)
#endif
{
    	HOOK_INIT
	
	static STRPTR t1 = "\033b\033uNewsgroups", t2 = "\033b\033uFlags", t3 = "subscribed", t4 = "\0", t5 = "\033b\033uCnt";
	static char buf[10];

	if ( msg->TreeNode != NULL )
	{
		/*
		**	The user data is a pointer to a SampleArray struct.
		*/
		struct SampleArray *a = (struct SampleArray *)msg->TreeNode->tn_User;

		sprintf( buf, "%3ld", msg->Array[-1] );

		*msg->Array++	= a->name;
		*msg->Array++	= ( a->flags & 0x8000 ) ? t3 : t4;
		*msg->Array++	= buf;
	}
	else
	{
		*msg->Array++	= t1;
		*msg->Array++	= t2;
		*msg->Array++	= t5;
	}

	return( 0 );
	
	HOOK_EXIT
}
MakeStaticHook(dsphook, dspfunc);

STATIC VOID StyleChanged( struct NListtreeP_Data *data, ULONG style )
{
	BOOL bp1=FALSE, bp2=FALSE, bp3=FALSE, bp4=FALSE, bi1=FALSE, bi2=FALSE, bi3=FALSE;

	switch( style )
	{
		case MUICFGV_NListtree_Style_Normal:
		case MUICFGV_NListtree_Style_Inserted:
		case MUICFGV_NListtree_Style_Mac:

			bp1 = TRUE; bp2 = TRUE; bp3 = TRUE; bp4 = TRUE; bi1 = FALSE; bi2 = FALSE; bi3 = TRUE;
			break;


		case MUICFGV_NListtree_Style_Lines:
		case MUICFGV_NListtree_Style_Win98:

			bp1 = FALSE; bp2 = TRUE; bp3 = TRUE; bp4 = TRUE; bi1 = FALSE; bi2 = FALSE; bi3 = TRUE;
			break;


		case MUICFGV_NListtree_Style_Lines3D:

			bp1 = FALSE; bp2 = FALSE; bp3 = TRUE; bp4 = TRUE; bi1 = FALSE; bi2 = FALSE; bi3 = TRUE;
			break;

		case MUICFGV_NListtree_Style_Win98Plus:

			bp1 = FALSE; bp2 = TRUE; bp3 = TRUE; bp4 = TRUE; bi1 = FALSE; bi2 = FALSE; bi3 = FALSE;
			break;

		case MUICFGV_NListtree_Style_Glow:

			bp1 = FALSE; bp2 = FALSE; bp3 = FALSE; bp4 = TRUE; bi1 = FALSE; bi2 = FALSE; bi3 = TRUE;
			break;
	}

	nnset( data->PP_LinePen,		MUIA_Disabled, bp1 );
	nnset( data->PP_ShadowPen,		MUIA_Disabled, bp2 );
	nnset( data->PP_DrawPen,		MUIA_Disabled, bp3 );
	nnset( data->PP_Draw2Pen,		MUIA_Disabled, bp4 );

	nnset( data->PI_ImageClosed,	MUIA_Disabled, bi1 );
	nnset( data->PI_ImageOpen,		MUIA_Disabled, bi2 );
	nnset( data->PI_ImageSpecial,	MUIA_Disabled, bi3 );

	return;
}

#ifdef __AROS__
AROS_HOOKPROTONHNO(StyleChangedFunc, VOID, ULONG **, para)
#else
HOOKPROTONHNO(StyleChangedFunc, VOID, ULONG **para)
#endif
{
    	HOOK_INIT
	
	StyleChanged( (struct NListtreeP_Data *)para[1], (ULONG)para[0] );
	
	HOOK_EXIT
}
MakeStaticHook(StyleChangedHook, StyleChangedFunc);

#ifdef __AROS__
AROS_HOOKPROTONHNO(TransferFunc, VOID, ULONG **, para)
#else
HOOKPROTONHNO(TransferFunc, VOID, ULONG **para)
#endif
{
    	HOOK_INIT
	
	TransferValues( (struct NListtreeP_Data *)para[0] );
	
	HOOK_EXIT
}
MakeStaticHook(TransferHook, TransferFunc);

ULONG _NewP( struct IClass *cl, Object *obj, Msg msg )
{
	struct NListtreeP_Data *data;
	STATIC UBYTE copytext[512];
	STATIC STRPTR CY_Style_Entries[9], STR_GR_Prefs[3];
	STATIC UBYTE	msg_bt_expand_key, msg_bt_collapse_key, msg_closed_key, msg_open_key, msg_special_key, msg_lines_key,
					msg_shadow_key, msg_draw_key, msg_draw2_key, msg_style_key, msg_space_key, msg_remember_status_key, msg_open_autoscroll_key,
					msg_bt_opensample_key, msg_bt_opencopyright_key, msg_bt_close_key;

	if ( !( obj = (Object *)DoSuperMethodA( cl, obj, msg ) ) )
		return( 0 );

	D(bug( "\n" ) );


	/*
	**	Init data.
	*/
	data = INST_DATA( cl, obj );

	if((LocaleBase = (APTR)OpenLibrary( "locale.library", 38)) &&
     GETINTERFACE(ILocale, LocaleBase))
	{
		Locale = OpenLocale( NULL );

		OpenCat( Locale );

		CY_Style_Entries[0] = LS( MSG_STYLE_NORMAL, "Normal" );
		CY_Style_Entries[1] = LS( MSG_STYLE_INSERTED, "Inserted" );
		CY_Style_Entries[2] = LS( MSG_STYLE_LINES, "Lines" );
		CY_Style_Entries[3] = LS( MSG_STYLE_WIN98, "Win98" );
		CY_Style_Entries[4] = LS( MSG_STYLE_MAC, "Mac" );
		CY_Style_Entries[5] = LS( MSG_STYLE_LINES3D, "Lines 3D" );
		CY_Style_Entries[6] = LS( MSG_STYLE_WIN98PLUS, "Win98+" );
		CY_Style_Entries[7] = LS( MSG_STYLE_GLOW, "Glow" );
		CY_Style_Entries[8] = NULL;

		STR_GR_Prefs[0]		= LS( MSG_STR_PREFS_IMAGESLINES, "Images/Lines" );
		STR_GR_Prefs[1]		= LS( MSG_STR_PREFS_STYLE, "Style" );
		STR_GR_Prefs[2]		= NULL;

		sprintf(copytext,
      "\033c\033bNListtree.mcp " LIB_REV_STRING " (" LIB_DATE ") \033n\n"
      "\033c" LIB_COPYRIGHT "\n"
      "\033cAll rights reserved.\n\n"
      "For developer information, bug reports and suggestions please visit the "
      "official homepage of the NList classes project:\n\n"
      "\033c\033bhttp://nlist-classes.sourceforge.net/\033n\n");

		msg_bt_expand_key			= LSC( MSG_BT_EXPAND_KEY, "n" );
		msg_bt_collapse_key			= LSC( MSG_BT_COLLAPSE_KEY, "e" );
		msg_closed_key				= LSC( MSG_CLOSED_KEY, "c" );
		msg_open_key				= LSC( MSG_OPEN_KEY, "o" );
		msg_special_key				= LSC( MSG_SPECIAL_KEY, "a" );
		msg_lines_key				= LSC( MSG_LINES_KEY, "l" );
		msg_shadow_key				= LSC( MSG_SHADOW_KEY, "w" );
		msg_draw_key				= LSC( MSG_DRAW_KEY, "d" );
		msg_draw2_key				= LSC( MSG_DRAW2_KEY, "2" );
		msg_style_key				= LSC( MSG_STYLE_KEY, "y" );
		msg_space_key				= LSC( MSG_SPACE_KEY, "p" );
		msg_remember_status_key		= LSC( MSG_REMEMBER_STATUS_KEY, "m" );
		msg_open_autoscroll_key		= LSC( MSG_OPEN_AUTOSCROLL_KEY, "s" );
		msg_bt_opensample_key		= LSC( MSG_BT_OPENSAMPLE_KEY, "b" );
		msg_bt_opencopyright_key	= LSC( MSG_BT_OPENCOPYRIGHT_KEY, "r" );
		msg_bt_close_key			= LSC( MSG_BT_CLOSE_KEY, "c" );


		data->WI_Sample = WindowObject,
			MUIA_Window_Title, LS( MSG_SAMPLE, "NListtree sample" ),
			MUIA_Window_ID, MAKE_ID( 'N','L','T','S' ),
			MUIA_Window_RootObject, VGroup,

				Child, MUI_MakeObject( MUIO_BarTitle, LS( MSG_SAMPLE, "NListtree sample" ) ),

				Child, NListviewObject,
					MUIA_ShortHelp,			LS( MSG_LV_SHORTHELP, "This NListtree object reacts on\npreferences changes on the right." ),
					MUIA_NListview_NList,	data->NLT_Sample = NListtreeObject,
						MUIA_Frame,					MUIV_Frame_InputList,
						MUIA_CycleChain,			TRUE,
						MUIA_NListtree_IsMCP,		TRUE,
						MUIA_NListtree_DisplayHook,	&dsphook,
						MUIA_NListtree_CompareHook,	MUIV_NListtree_CompareHook_LeavesTop,
						MUIA_NListtree_DoubleClick,	MUIV_NListtree_DoubleClick_All,
						MUIA_NListtree_EmptyNodes,	FALSE,
						MUIA_NListtree_TreeColumn,	0,
						MUIA_NListtree_Title,		TRUE,
						MUIA_NListtree_Format,		",",
					End,
				End,

				Child, HGroup,
					Child, data->BT_Sample_Expand	= KeyButton( LS( MSG_BT_EXPAND, "Expand" ), msg_bt_expand_key ),
					Child, data->BT_Sample_Collapse	= KeyButton( LS( MSG_BT_COLLAPSE, "Collapse" ), msg_bt_collapse_key ),
					Child, data->BT_Sample_Close	= KeyButton( LS( MSG_BT_CLOSE, "Close" ), msg_bt_close_key ),
				End,
			End,
		End;

		data->WI_Copyright = WindowObject,
			MUIA_Window_Title, LS( MSG_COPYRIGHT, "Copyright" ),
			MUIA_Window_ID, MAKE_ID( 'N','L','T','C' ),
			MUIA_Window_RootObject, VGroup,

				Child, NListviewObject,
					MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_None,
					MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_None,
					MUIA_NListview_NList, NFloattextObject,
						MUIA_NFloattext_Text, copytext,
						MUIA_NFloattext_TabSize, 4,
						MUIA_NFloattext_Justify, TRUE,
					End,
				End,

				Child, HGroup,
					Child, HVSpace,
					Child, data->BT_Copyright_Close = KeyButton( LS( MSG_BT_CLOSE, "Close" ), msg_bt_close_key ),
					Child, HVSpace,
				End,
			End,

		End;


		/*
		**	Preferences group.
		*/
		data->GR_Prefs = VGroup,
			MUIA_Group_VertSpacing, 5,

			Child, MUI_MakeObject( MUIO_BarTitle, LS( MSG_PREFS, "Preferences" ) ),

			Child, RegisterObject,
				MUIA_Register_Titles, STR_GR_Prefs,

				/*
				**	Images and Lines
				*/
				Child, HGroup,

					Child, VGroup,

						Child, FreeKeyCLabel( LS( MSG_CLOSED, "Closed:" ), msg_closed_key ),

						Child, data->PI_ImageClosed = MUI_NewObject( MUIC_Popimage,
							MUIA_Imagedisplay_UseDefSize,	 TRUE,
							MUIA_Imageadjust_Type,		MUIV_Imageadjust_Type_All,
							MUIA_ControlChar,			msg_closed_key,
							MUIA_CycleChain,			TRUE,
							MUIA_Draggable,				TRUE,
							MUIA_Window_Title,			LS( MSG_WIN_TITLE_CLOSED_IMAGE, "Adjust 'closed node' image" ),
							MUIA_ShortHelp,				LS( MSG_SHORTHELP_CLOSED_IMAGE, "Image for a closed node." ),
						End,


						Child, FreeKeyCLabel( LS( MSG_OPEN, "Open:" ), msg_open_key ),

						Child, data->PI_ImageOpen = MUI_NewObject( MUIC_Popimage,
							MUIA_Imagedisplay_UseDefSize,	 TRUE,
							MUIA_Imageadjust_Type,		MUIV_Imageadjust_Type_All,
							MUIA_ControlChar,			msg_open_key,
							MUIA_CycleChain,			TRUE,
							MUIA_Draggable,				TRUE,
							MUIA_Window_Title,			LS( MSG_WIN_TITLE_OPEN_IMAGE, "Adjust 'opened node' image" ),
							MUIA_ShortHelp,				LS( MSG_SHORTHELP_OPEN_IMAGE, "Image for an opened node." ),
						End,

						Child, FreeKeyCLabel( LS( MSG_SPECIAL, "Special:" ), msg_special_key ),

						Child, data->PI_ImageSpecial = MUI_NewObject( MUIC_Popimage,
							MUIA_Imagedisplay_UseDefSize,	 TRUE,
							MUIA_Imageadjust_Type,		MUIV_Imageadjust_Type_All,
							MUIA_ControlChar,			msg_special_key,
							MUIA_CycleChain,			TRUE,
							MUIA_Draggable,				TRUE,
							MUIA_Window_Title,			LS( MSG_WIN_TITLE_SPECIAL_IMAGE, "Adjust special image" ),
							MUIA_ShortHelp,				LS( MSG_SHORTHELP_SPECIAL_IMAGE, "Special image." ),
						End,

					End,


					Child, VGroup,

						Child, FreeKeyCLabel( LS( MSG_LINES, "Lines:" ), msg_lines_key ),

						Child, data->PP_LinePen = MUI_NewObject( MUIC_Poppen,
							MUIA_ControlChar,	msg_lines_key,
							MUIA_CycleChain,	TRUE,
							MUIA_Window_Title,	LS( MSG_WIN_TITLE_LINEPEN, "Adjust `Lines' pen" ),
							MUIA_ShortHelp,		LS( MSG_SHORTHELP_LINEPEN, "Custom line color." ),
						End,


						Child, FreeKeyCLabel( LS( MSG_SHADOW, "Shadow:" ), msg_shadow_key ),

						Child, data->PP_ShadowPen = MUI_NewObject( MUIC_Poppen,
							MUIA_ControlChar,	msg_shadow_key,
							MUIA_CycleChain,	TRUE,
							MUIA_Window_Title,	LS( MSG_WIN_TITLE_SHADOWPEN, "Adjust `Shadow' pen" ),
							MUIA_ShortHelp,		LS( MSG_SHORTHELP_SHADOWPEN, "Custom shadow color." ),
						End,

						Child, FreeKeyCLabel( LS( MSG_DRAW, "Draw:" ), msg_draw_key ),

						Child, data->PP_DrawPen = MUI_NewObject( MUIC_Poppen,
							MUIA_Disabled,		TRUE,
							MUIA_ControlChar,	msg_draw_key,
							MUIA_CycleChain,	TRUE,
							MUIA_Window_Title,	LS( MSG_WIN_TITLE_DRAWPEN, "Adjust `Draw' pen" ),
							MUIA_ShortHelp,		LS( MSG_SHORTHELP_DRAWPEN, "Custom draw color." ),
						End,

						//Child, FreeKeyCLabel( LS( MSG_DRAW2, "Draw2:" ), msg_draw2_key ),

						Child, data->PP_Draw2Pen = MUI_NewObject( MUIC_Poppen,
							MUIA_ShowMe,		FALSE,
							MUIA_Disabled,		TRUE,
							MUIA_ControlChar,	msg_draw2_key,
							MUIA_CycleChain,	TRUE,
							MUIA_Window_Title,	LS( MSG_WIN_TITLE_DRAWPEN, "Adjust `Draw2' pen" ),
							MUIA_ShortHelp,		LS( MSG_SHORTHELP_DRAWPEN, "Custom draw color 2." ),
						End,

					End,

				End,


				/*
				**	Style
				*/
				Child, HGroup,

					Child, ColGroup( 2 ),

						Child, KeyLabel( LS( MSG_STYLE, "Style:" ), msg_style_key ),

						Child, data->CY_Style = CycleObject,
							MUIA_Cycle_Entries,	CY_Style_Entries,
							MUIA_Cycle_Active,	0,
							MUIA_ShortHelp,		LS( MSG_SHORTHELP_STYLE, "Global style of the tree." ),
						End,


						Child, KeyLabel( LS( MSG_SPACE, "Space:" ), msg_space_key ),

						Child, data->SL_Space = SliderObject,
							MUIA_Group_Horiz,		TRUE,
							MUIA_Slider_Min,		0,
							MUIA_Slider_Max,		16,
							MUIA_Slider_Level,		0,
							MUIA_Numeric_Format,	"%ldpx",
							MUIA_ShortHelp,			LS( MSG_SHORTHELP_SPACE, "Number of space pixels." ),
						End,

					End,


					Child, ColGroup( 2 ),

						Child, KeyLabel( LS( MSG_REMEMBER_STATUS, "Remember:" ), msg_remember_status_key ),

						Child, data->CH_RememberStatus = ImageObject,
							MUIA_Image_Spec,		MUII_CheckMark,
							MUIA_InputMode,			MUIV_InputMode_Toggle,
							MUIA_Selected,			FALSE,
							MUIA_ShowSelState,		FALSE,
							MUIA_ControlChar,		msg_remember_status_key,
							MUIA_Background,		MUII_ButtonBack,
							ImageButtonFrame,
							MUIA_ShortHelp,			LS( MSG_SHORTHELP_REMEMBER_STATUS, "Remember status of nodes\nafter closing." ),
						End,


						Child, KeyLabel( LS( MSG_OPEN_AUTOSCROLL, "Open scroll:" ), msg_open_autoscroll_key ),

						Child, data->CH_OpenAutoScroll = ImageObject,
							MUIA_Image_Spec,		MUII_CheckMark,
							MUIA_InputMode,			MUIV_InputMode_Toggle,
							MUIA_Selected,			FALSE,
							MUIA_ShowSelState,		FALSE,
							MUIA_ControlChar,		msg_open_autoscroll_key,
							MUIA_Background,		MUII_ButtonBack,
							ImageButtonFrame,
							MUIA_ShortHelp,			LS( MSG_SHORTHELP_OPEN_AUTOSCROLL, "Auto scroll listview when opening\nnodes to fit in visible area." ),
						End,

					End,

				End,

			End,

			Child, HGroup,
				MUIA_Frame, MUIV_Frame_Group,
				Child, data->BT_OpenSample		= KeyButton( LS( MSG_BT_OPENSAMPLE, "Sample (b)" ), msg_bt_opensample_key ),
				Child, data->BT_OpenCopyright	= KeyButton( LS( MSG_BT_OPENCOPYRIGHT, "Copyright" ), msg_bt_opencopyright_key ),
				//Child, data->BT_Test			= KeyButton( "TEST", "" ),
			End,

		End;


		if ( data->GR_Prefs == NULL )
		{
			CoerceMethod( cl, obj, OM_DISPOSE );
			return( 0 );
		}


		DrawSampleTree( data->NLT_Sample );

		DoMethod( obj, OM_ADDMEMBER, data->GR_Prefs );


		/*
		**	Sub windows
		*/
		DoMethod( data->BT_OpenSample, MUIM_Notify, MUIA_Pressed, FALSE,
			data->WI_Sample, 3, MUIM_Set, MUIA_Window_Open, TRUE );

		DoMethod( data->BT_OpenSample, MUIM_Notify, MUIA_Pressed, FALSE,
			data->WI_Sample, 4, MUIM_CallHook, &TransferHook, data );

		DoMethod( data->BT_OpenCopyright, MUIM_Notify, MUIA_Pressed, FALSE,
			data->WI_Copyright, 3, MUIM_Set, MUIA_Window_Open, TRUE );

		DoMethod( data->WI_Sample, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			data->WI_Sample, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( data->WI_Copyright, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			data->WI_Copyright, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( data->BT_Sample_Close, MUIM_Notify, MUIA_Pressed, FALSE,
			data->WI_Sample, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( data->BT_Copyright_Close, MUIM_Notify, MUIA_Pressed, FALSE,
			data->WI_Copyright, 3, MUIM_Set, MUIA_Window_Open, FALSE );


		/*
		**	Open/close all nodes
		*/
		DoMethod( data->BT_Sample_Expand, MUIM_Notify, MUIA_Pressed, FALSE,
			data->NLT_Sample, 4, MUIM_NListtree_Open, MUIV_NListtree_Open_ListNode_Root, MUIV_NListtree_Open_TreeNode_All, 0 );

		DoMethod( data->BT_Sample_Collapse, MUIM_Notify, MUIA_Pressed, FALSE,
			data->NLT_Sample, 4, MUIM_NListtree_Close, MUIV_NListtree_Close_ListNode_Root, MUIV_NListtree_Close_TreeNode_All, 0 );

		/*
		DoMethod( data->BT_Test, MUIM_Notify, MUIA_Pressed, FALSE,
			data->NLT_Sample, 4, MUIM_NListtree_Close, MUIV_NListtree_Close_ListNode_Root, MUIV_NListtree_Close_TreeNode_All, 0 );
		*/

		/*
		**	Open/closed node and special images
		*/
		DoMethod( data->PI_ImageClosed, MUIM_Notify, MUIA_Imagedisplay_Spec, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_ImageSpecClosed, MUIV_TriggerValue );

		DoMethod( data->PI_ImageOpen, MUIM_Notify, MUIA_Imagedisplay_Spec, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_ImageSpecOpen, MUIV_TriggerValue );

		DoMethod( data->PI_ImageSpecial, MUIM_Notify, MUIA_Imagedisplay_Spec, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_ImageSpecSpecial, MUIV_TriggerValue );


		/*
		**	Colors
		*/
		DoMethod( data->PP_LinePen, MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_PenSpecLines, MUIV_TriggerValue );

		DoMethod( data->PP_ShadowPen, MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_PenSpecShadow, MUIV_TriggerValue );

		DoMethod( data->PP_DrawPen, MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_PenSpecDraw, MUIV_TriggerValue );

		DoMethod( data->PP_Draw2Pen, MUIM_Notify, MUIA_Pendisplay_Spec, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_PenSpecDraw2, MUIV_TriggerValue );


		/*
		**	Values
		*/
		DoMethod( data->CY_Style, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_Style, MUIV_TriggerValue );

		DoMethod( data->CY_Style, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
			data->NLT_Sample, 4, MUIM_CallHook, &StyleChangedHook, MUIV_TriggerValue, data );

		DoMethod( data->SL_Space, MUIM_Notify, MUIA_Slider_Level, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_Space, MUIV_TriggerValue );

		DoMethod( data->CH_RememberStatus, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_RememberStatus, MUIV_TriggerValue );

		DoMethod( data->CH_OpenAutoScroll, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			data->NLT_Sample, 3, MUIM_Set, MUICFG_NListtree_OpenAutoScroll, MUIV_TriggerValue );

		return( (ULONG)obj );
	}

	return(0);
}


ULONG _DisposeP( struct IClass *cl, Object *obj, Msg msg )
{
	struct NListtreeP_Data *data = INST_DATA( cl, obj );

	D(bug( "\n" ) );

	CloseCat();

	if ( Locale )
		CloseLocale( Locale );

	if(LocaleBase)
  {
    DROPINTERFACE(ILocale);
		CloseLibrary((struct Library *)LocaleBase);
  }

	DoMethod( obj, OM_REMMEMBER, data->GR_Prefs );

	if ( data->WI_Sample )
		MUI_DisposeObject( data->WI_Sample );

	if ( data->WI_Copyright )
		MUI_DisposeObject( data->WI_Copyright );

	if ( data->GR_Prefs )
		MUI_DisposeObject( data->GR_Prefs );

	return( DoSuperMethodA( cl, obj, msg ) );
}


ULONG _SetupP( struct IClass *cl, Object *obj, struct MUIP_Setup *msg )
{
	struct NListtreeP_Data *data = INST_DATA( cl, obj );

	if ( !( DoSuperMethodA( cl, obj, (Msg)msg ) ) )
		return( FALSE );

	D(bug( "\n" ) );

	DoMethod( _app( obj ), OM_ADDMEMBER, data->WI_Sample );
	DoMethod( _app( obj ), OM_ADDMEMBER, data->WI_Copyright );

	if ( data->SampleWasOpen )
		set( data->WI_Sample, MUIA_Window_Open, TRUE );

	if ( data->CopyrightWasOpen )
		set( data->WI_Copyright, MUIA_Window_Open, TRUE );

	data->SampleWasOpen = FALSE;
	data->CopyrightWasOpen = FALSE;

	/*
	**	Values
	**	We set the values here directly to the NL, because
	**	notification is only done, if values != old values.
	*/
	TransferValues( data );

	return( TRUE );
}


ULONG _CleanupP( struct IClass *cl, Object *obj, struct MUIP_Setup *msg )
{
	struct NListtreeP_Data *data = INST_DATA(cl, obj);

	D(bug( "\n" ) );

	if((data->SampleWasOpen = xget(data->WI_Sample, MUIA_Window_Open)))
	{
		set( data->WI_Sample, MUIA_Window_Open, FALSE );
	}

	if((data->CopyrightWasOpen = xget( data->WI_Copyright, MUIA_Window_Open)))
	{
		set( data->WI_Copyright, MUIA_Window_Open, FALSE );
	}

	DoMethod( _app( obj ), OM_REMMEMBER, data->WI_Sample );
	DoMethod( _app( obj ), OM_REMMEMBER, data->WI_Copyright );

	return( DoSuperMethodA( cl, obj, (Msg)msg ) );
}


ULONG _ConfigToGadgets( struct IClass *cl, Object *obj, struct MUIP_Settingsgroup_ConfigToGadgets *msg )
{
	struct NListtreeP_Data *data = INST_DATA(cl, obj);
	Object *pdobj, *idobj;
	struct MUI_ImageSpec *is;
	struct MUI_PenSpec *pen;
	ULONG d;

	D(bug( "\n" ) );

	/*
	**	Create objects
	*/
	pdobj = MUI_NewObject( MUIC_Pendisplay,		TAG_DONE );
	idobj = MUI_NewObject( MUIC_Imagedisplay,	TAG_DONE );

	D(bug( "pdobj: 0x%08lx, idobj: 0x%08lx\n", pdobj, idobj ) );


	/*
	**	Set images
	*/
	if((is = (struct MUI_ImageSpec *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_ImageSpecClosed)))
	{
		set( data->PI_ImageClosed, MUIA_Imagedisplay_Spec, is );

		D(bug( "Closed node image: '%s'\n", (STRPTR)is ) );
	}
	else
	{
		if ( idobj )
		{
			DoMethod( idobj, MUIM_Set, MUIA_Imagedisplay_Spec, "1:17" );

			get( idobj, MUIA_Imagedisplay_Spec, &is );
			set( data->PI_ImageClosed, MUIA_Imagedisplay_Spec, is );

			D(bug( "Closed node image: '%s'\n", (STRPTR)is ) );
		}
	}


	if((is = (struct MUI_ImageSpec *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_ImageSpecOpen)))
	{
		set( data->PI_ImageOpen, MUIA_Imagedisplay_Spec, is );

		D(bug( "Open node image: '%s'\n", (STRPTR)is ) );
	}
	else
	{
		if ( idobj )
		{
			DoMethod( idobj, MUIM_Set, MUIA_Imagedisplay_Spec, "1:23" );

			get( idobj, MUIA_Imagedisplay_Spec, &is );
			set( data->PI_ImageOpen, MUIA_Imagedisplay_Spec, is );

			D(bug( "Open node image: '%s'\n", (STRPTR)is ) );
		}
	}


	if((is = (struct MUI_ImageSpec *)DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_ImageSpecSpecial)))
	{
		set( data->PI_ImageSpecial, MUIA_Imagedisplay_Spec, is );

		D(bug( "Special node image: '%s'\n", (STRPTR)is ) );
	}
	else
	{
		if ( idobj )
		{
			DoMethod( idobj, MUIM_Set, MUIA_Imagedisplay_Spec, "1:9" );

			get( idobj, MUIA_Imagedisplay_Spec, &is );
			set( data->PI_ImageSpecial, MUIA_Imagedisplay_Spec, is );

			D(bug( "Special node image: '%s'\n", (STRPTR)is ) );
		}
	}


	/*
	**	Set colors
	*/
	if((d = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_PenSpecLines)))
	{
		set( data->PP_LinePen, MUIA_Pendisplay_Spec, d );

		D(bug( "Line color: '%s'\n", (STRPTR)d ) );
	}
	else
	{
		if( pdobj )
		{
			DoMethod( pdobj, MUIM_Pendisplay_SetMUIPen, DEFAULT_PEN_LINES );

			get( pdobj, MUIA_Pendisplay_Spec, &pen );
			set( data->PP_LinePen, MUIA_Pendisplay_Spec, pen );

			D(bug( "Line color: '%s'\n", pen ) );
		}
	}

	if((d = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_PenSpecShadow)))
	{
		set( data->PP_ShadowPen, MUIA_Pendisplay_Spec, d );

		D(bug( "Shadow color: '%s'\n", (STRPTR)d ) );
	}
	else
	{
		if( pdobj )
		{
			DoMethod( pdobj, MUIM_Pendisplay_SetMUIPen, DEFAULT_PEN_SHADOW );

			get( pdobj, MUIA_Pendisplay_Spec, &pen );
			set( data->PP_ShadowPen, MUIA_Pendisplay_Spec, pen );

			D(bug( "Shadow color: '%s'\n", pen ) );
		}
	}

	if((d = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_PenSpecDraw)))
	{
		set( data->PP_DrawPen, MUIA_Pendisplay_Spec, d );

		D(bug( "Draw color: '%s'\n", (STRPTR)d ) );
	}
	else
	{
		if( pdobj )
		{
			DoMethod( pdobj, MUIM_Pendisplay_SetMUIPen, DEFAULT_PEN_DRAW );

			get( pdobj, MUIA_Pendisplay_Spec, &pen );
			set( data->PP_DrawPen, MUIA_Pendisplay_Spec, pen );

			D(bug( "Draw color: '%s'\n", pen ) );
		}
	}


	if((d = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_PenSpecDraw2)))
	{
		set( data->PP_Draw2Pen, MUIA_Pendisplay_Spec, d );

		D(bug( "Draw color 2: '%s'\n", (STRPTR)d ) );
	}
	else
	{
		if( pdobj )
		{
			DoMethod( pdobj, MUIM_Pendisplay_SetMUIPen, DEFAULT_PEN_DRAW );

			get( pdobj, MUIA_Pendisplay_Spec, &pen );
			set( data->PP_Draw2Pen, MUIA_Pendisplay_Spec, pen );

			D(bug( "Draw color 2: '%s'\n", pen ) );
		}
	}


	/*
	**	Set values
	*/
	if((d = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_Style)))
	{
		set( data->CY_Style, MUIA_Cycle_Active, atoi( (STRPTR)d ) );
		StyleChanged( data, atoi( (STRPTR)d ) );

		D(bug( "Style: %ld\n", atoi( (STRPTR)d ) ) );
	}
	else
	{
		set( data->CY_Style, MUIA_Cycle_Active, MUICFGV_NListtree_Style_Lines3D );
		StyleChanged( data, MUICFGV_NListtree_Style_Lines3D );

		D(bug( "Style: %ld\n", 0 ) );
	}

	if((d = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_Space)))
	{
		set( data->SL_Space, MUIA_Slider_Level, atoi( (STRPTR)d ) );

		D(bug( "Space: %ld\n", atoi( (STRPTR)d ) ) );
	}
	else
	{
		set( data->SL_Space, MUIA_Slider_Level, MUICFGV_NListtree_Space_Default );

		D(bug( "Space: %ld\n", MUICFGV_NListtree_Space_Default ) );
	}


	if((d = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_RememberStatus)))
	{
		set( data->CH_RememberStatus, MUIA_Selected, atoi( (STRPTR)d ) );

		D(bug( "RememberStatus: %ld\n", atoi( (STRPTR)d ) ) );
	}
	else
	{
		set( data->CH_RememberStatus, MUIA_Selected, TRUE );

		D(bug( "RememberStatus: 1\n" ) );
	}


	if((d = DoMethod(msg->configdata, MUIM_Dataspace_Find, MUICFG_NListtree_OpenAutoScroll)))
	{
		set( data->CH_OpenAutoScroll, MUIA_Selected, atoi( (STRPTR)d ) );

		D(bug( "OpenAutoScroll: %ld\n", atoi( (STRPTR)d ) ) );
	}
	else
	{
		set( data->CH_OpenAutoScroll, MUIA_Selected, TRUE );

		D(bug( "OpenAutoScroll: 1\n" ) );
	}

	/*
	**	Dispose created objects
	*/
	if( pdobj )
		MUI_DisposeObject( pdobj );

	return( 0 );
}


ULONG _GadgetsToConfig( struct IClass *cl, Object *obj, struct MUIP_Settingsgroup_GadgetsToConfig *msg )
{
	struct NListtreeP_Data *data = INST_DATA( cl, obj );
	char buf[5];
	ULONG d;

	D(bug( "\n" ) );

	/*
	**	Images
	*/
	get( data->PI_ImageClosed,	MUIA_Imagedisplay_Spec,	&d );
	if( d != 0)
		DoMethod( msg->configdata, MUIM_Dataspace_Add, d, sizeof( struct MUI_ImageSpec ), MUICFG_NListtree_ImageSpecClosed );

	D(bug( "Image closed: '%s'\n", (STRPTR)d ) );


	get( data->PI_ImageOpen,	MUIA_Imagedisplay_Spec,	&d );
	if(d != 0)
		DoMethod( msg->configdata, MUIM_Dataspace_Add, d, sizeof( struct MUI_ImageSpec ), MUICFG_NListtree_ImageSpecOpen );

	D(bug( "Image open: '%s'\n", (STRPTR)d ) );


	get( data->PI_ImageSpecial,	MUIA_Imagedisplay_Spec,	&d );
	if(d != 0)
		DoMethod( msg->configdata, MUIM_Dataspace_Add, d, sizeof( struct MUI_ImageSpec ), MUICFG_NListtree_ImageSpecSpecial );

	D(bug( "Image special: '%s'\n", (STRPTR)d ) );


	/*
	**	Colors
	*/
	get( data->PP_LinePen, MUIA_Pendisplay_Spec, &d );
	if(d != 0)
		DoMethod( msg->configdata, MUIM_Dataspace_Add, d, sizeof( struct MUI_PenSpec ), MUICFG_NListtree_PenSpecLines );

	D(bug( "Line color: '%s'\n", (STRPTR)d ) );


	get( data->PP_ShadowPen, MUIA_Pendisplay_Spec, &d );
	if(d != 0)
		DoMethod( msg->configdata, MUIM_Dataspace_Add, d, sizeof( struct MUI_PenSpec ), MUICFG_NListtree_PenSpecShadow );

	D(bug( "Shadow color: '%s'\n", (STRPTR)d ) );


	get( data->PP_DrawPen, MUIA_Pendisplay_Spec, &d );
	if(d != 0)
		DoMethod( msg->configdata, MUIM_Dataspace_Add, d, sizeof( struct MUI_PenSpec ), MUICFG_NListtree_PenSpecDraw );

	D(bug( "Draw color: '%s'\n", (STRPTR)d ) );


	get( data->PP_Draw2Pen, MUIA_Pendisplay_Spec, &d );
	if(d != 0)
		DoMethod( msg->configdata, MUIM_Dataspace_Add, d, sizeof( struct MUI_PenSpec ), MUICFG_NListtree_PenSpecDraw2 );

	D(bug( "Draw color 2: '%s'\n", (STRPTR)d ) );


	/*
	**	Values
	*/
	get( data->CY_Style, MUIA_Cycle_Active, &d );
	sprintf( buf, "%ld", d );
	DoMethod( msg->configdata, MUIM_Dataspace_Add, buf, 5, MUICFG_NListtree_Style );

	D(bug( "Style: %ld\n", d ) );


	get( data->SL_Space, MUIA_Slider_Level, &d );
	sprintf( buf, "%ld", d );
	DoMethod( msg->configdata, MUIM_Dataspace_Add, buf, 5, MUICFG_NListtree_Space );

	D(bug( "Space: %ld\n", d ) );


	get( data->CH_RememberStatus, MUIA_Selected, &d );
	sprintf( buf, "%ld", d );
	DoMethod( msg->configdata, MUIM_Dataspace_Add, buf, 5, MUICFG_NListtree_RememberStatus );

	D(bug( "RememberStatus: %ld\n", d ) );


	get( data->CH_OpenAutoScroll, MUIA_Selected, &d );
	sprintf( buf, "%ld", d );
	DoMethod( msg->configdata, MUIM_Dataspace_Add, buf, 5, MUICFG_NListtree_OpenAutoScroll );

	D(bug( "OpenAutoScroll: %ld\n", d ) );

	return( 0 );
}


ULONG _HandleInputP( struct IClass *cl, Object *obj, struct MUIP_HandleInput *msg )
{
	if( msg->imsg )
	{
		switch( msg->imsg->Class )
		{
			case IDCMP_INTUITICKS:
				break;
		}
	}

	return( DoSuperMethodA( cl, obj, (Msg)msg ) );
}

DISPATCHERPROTO(_DispatcherP)
{
    	DISPATCHER_INIT
	
	switch( msg->MethodID )
	{
		case OM_NEW:								return( _NewP				( cl, obj, (APTR)msg ) );
		case OM_DISPOSE:							return( _DisposeP			( cl, obj, (APTR)msg ) );

		case MUIM_Setup:							return( _SetupP				( cl, obj, (APTR)msg ) );
		case MUIM_Cleanup:							return( _CleanupP			( cl, obj, (APTR)msg ) );

		case MUIM_HandleInput:						return( _HandleInputP		( cl, obj, (APTR)msg ) );

		case MUIM_Settingsgroup_ConfigToGadgets:	return( _ConfigToGadgets	( cl, obj, (APTR)msg ) );
		case MUIM_Settingsgroup_GadgetsToConfig:	return( _GadgetsToConfig	( cl, obj, (APTR)msg ) );
	}

	return( DoSuperMethodA( cl, obj, msg ) );
	
	DISPATCHER_EXIT
}


