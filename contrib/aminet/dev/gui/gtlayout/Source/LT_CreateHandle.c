#include <stdio.h>
/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

/*****************************************************************************/

#include <exec/memory.h>

#include <clib/keymap_protos.h>
#include <pragmas/keymap_pragmas.h>

#include <stdarg.h>

/*****************************************************************************/

#include "Assert.h"

LayoutHandle * LIBENT
LT_CreateHandle(REG(a0) struct Screen *Screen,REG(a1) struct TextAttr *Font)
{
	return(LT_CreateHandleTags(Screen,
		LH_Font,Font,
	TAG_DONE));
}


/*****************************************************************************/

#ifndef __AROS__
LayoutHandle *
LT_CreateHandleTags(struct Screen *Screen,...)
{
	LayoutHandle	*Handle;
	va_list 		 VarArgs;

	va_start(VarArgs,Screen);
	Handle = LT_CreateHandleTagList(Screen,(struct TagItem *)VarArgs);
	va_end(VarArgs);
	return(Handle);
}
#endif

/*****************************************************************************/


/****** gtlayout.library/LT_CreateHandleTagList ******************************************
*
*   NAME
*	LT_CreateHandleTagList -- Allocate auxilary data required by LT_New()
*	                          and LT_BuildA().
*
*   SYNOPSIS
*	Handle = LT_CreateHandleTagList(Screen,Tags);
*	  D0                              A0    A1
*
*	LayoutHandle *LT_CreateHandleTagList(struct Screen *,struct TagItem *);
*
*	Handle = LT_CreateHandleTags(Screen,...);
*
*	struct LayoutHandle *LT_CreateHandleTags(struct Screen *,...);
*
*   FUNCTION
*	Memory is allocated, tables are set up and data is collected
*	on a screen a user interface is to be opened on. This
*	involves calculating the screen font parameters.
*
*   INPUTS
*	Screen - Pointer to the screen the user interface is to
*	    use. Passing NULL will cause the default public
*	    screen to be used.
*
*	        NOTE: if NULL is passed the default public screen
*	            will stay locked until LT_DeleteHandle()
*	            is called.
*
*	Tags - Tag values to control certain aspects of the
*	    user interface created.
*
*
*	Valid tags include:
*
*	LAHN_AutoActivate (BOOL) - Set to TRUE if you want the interface
*	    to always keep a string gadget active if possible. Hitting
*	    the return key will then cause the next following string
*	    gadget to get activated, either cycling through all the
*	    string gadgets available or stopping at the next string
*	    gadget to have the LAST_LastGadget attribute set.
*	    Default: TRUE
*
*	LAHN_RawKeyFilter (BOOL) - Discard unprocessed IDCMP_RAWKEY
*	    events. (V13)
*	    Default: TRUE
*
*	LAHN_UserData (APTR) - Store user specific data in the
*	    LayoutHandle->UserData entry.
*
*	        NOTE: This tag requires gtlayout.library v9 and the
*	            corresponding entry in the LayoutHandle exists
*	            only under gtlayout.library v9 and up. *NEVER*
*	            write to this entry, use LT_SetAttributes()
*	            instead.
*
*	LAHN_LocaleHook (struct Hook *) - The hook to call when
*	    locale string IDs are to be mapped to strings. The
*	    hook function is called with the following parameters:
*
*	    String = HookFunc(struct Hook *Hook,struct LayoutHandle *Handle,
*	      D0                            A0                         A2
*	                      LONG ID)
*	                           A1
*
*	    The function is to look up the string associated with the ID
*	    passed in and return the string.
*	    Default: no locale hook
*
*	LAHN_TextAttr (struct TTextAttr *) - The text font to use when
*	    creating the gadgets and objects.
*	    Default: Screen->Font
*
*	LAHN_CloningPermitted (BOOL) - If a window will not fit onto the
*	    screen the user interface is intended for, the layout engine
*	    will scale the interface data down while stepping down in
*	    font size. If all this fails, the engine will open a custom
*	    screen for the window; this process is called "cloning".
*	    The LAHN_CloningPermitted tag controls whether the engine will
*	    actually try to open the custom screen or just return NULL
*	    when LT_Build fails.
*	    Default: TRUE
*
*	LAHN_EditHook (struct Hook *) - You can specify a default string
*	    gadget editing hook to be used for all following string
*	    gadgets. Your hook should obey the same rules that apply
*	    to hooks passed via GTST_EditHook/GTIN_EditHook.
*	    Default: NULL
*
*	LAHN_ExactClone (BOOL) - This tag works in conjunction with the
*	    LAHN_CloningPermitted tag. By default the layout engine will
*	    try to replicate only the basic characteristics of the
*	    screen the window was intended to open on. This may result
*	    in a screen which uses less colours than the original
*	    screen. You can force the engine to make an almost exact
*	    clone of the original screen by passing the LAHN_ExactClone
*	    tag with a value of TRUE.
*	    Default: FALSE
*
*	LAHN_MenuGlyphs (BOOL) - This tag will make the layout engine
*	    fill in the AmigaGlyph and CheckGlyph entries of the
*	    LayoutHandle if running under Kickstart 3.0 or higher.
*	    The corresponding images will be scaled to fit the actual
*	    screen aspect ratio values and can later be used for
*	    menu layout.
*	    Default: FALSE
*
*	LAHN_Parent (struct Window *) - You can pass a pointer to the
*	    parent window of the window you intend to open using
*	    the user interface layout engine. The new window will
*	    open inside the boundaries of the parent window. If the
*	    size does not fit, it will be opened centered over the
*	    parent window.
*	    Default: NULL
*
*	LAHN_BlockParent (BOOL) - This tag works in conjunction with the
*	    LAHN_Parent tag. If in effect, will block the parent window
*	    via LT_LockWindow until the new window is closed, after
*	    which the parent window is unlocked again.
*	    Default: FALSE
*
*	LAHN_SimpleClone (BOOL) - This tag works in conjunction with the
*	    LAHN_CloningPermitted tag. It will make the layout engine
*	    forget most information about the original screen the
*	    user interface was intended for. In short, it will open a
*	    simple default screen for the interface.
*	    Default: FALSE
*
*	LAHN_ExitFlush (BOOL) - When the LayoutHandle is finally disposed
*	    of with LT_DeleteHandle() all variables maintained by the
*	    input handling code will be flushed. For example, if you
*	    would use the LA_STRPTR tag for STRING_KIND objects the
*	    last string gadget contents would be copied into the buffer
*	    pointed to by LA_STRPTR. If you do not want to use this
*	    feature, disable it with "LAHN_ExitFlush,FALSE". (V9)
*	    Default: TRUE
*
*	LAHN_NoKeys (BOOL) - Use TRUE to tell the library not to pick
*	    keyboard shortcuts all on its own. This works like calling
*	    LT_New() for all objects with "LA_NoKey,TRUE,". (V26)
*
*	LAHN_PubScreen (struct Screen *) - Pointer to public screen
*		window is to open on. Must be locked and open until you call
*		LT_Built().
*
*	LAHN_PubScreenName (STRPTR) - Name of public screen to open window
*		on. The library will try to lock the named screen as soon as
*		you call LT_CreateHandle.
*
*	LAHN_PubScreenFallBack (BOOL) - If the named public screen cannot
*		be found and you ask for it, the library will lock the default
*		public screen (default: TRUE).
*
*	LAHN_TopGroupType (LONG) - Request that after creating the layout
*	    handle, a layout group should be added to it. This can be
*	    either a vertical or a horizontal group. You specify the type
*	    using VERTICAL_KIND or HORIZONTAL_KIND (default: don't add
*	    any group). (V45)
*
*   RESULT
*	Handle - Pointer to a LayoutHandle structure.
*
******************************************************************************
*
*/

LayoutHandle * LIBENT
LT_CreateHandleTagList(REG(a0) struct Screen *Screen,REG(a1) struct TagItem *TagList)
{
	LayoutHandle *	Handle;
	struct Screen *	PubScreen;
	APTR			Pool;
	ULONG			WA_ScreenTag;
	BOOL			UnlockThePubScreen;
	LONG			TopGroupType = -1; 

	#ifdef DO_PICKSHORTCUTS
	{
		ObtainSemaphore(&LTP_KeySemaphore);

		if(!LTP_Keys[1])
		{
			UBYTE mapBuffer[2 * 3];
			UBYTE remapBuffer[10];
			LONG i;
			struct InputEvent event;

			LTP_Keys[1] = &LTP_Keys[0][256];

			for(i = 32 ; i < 256; i++)
			{
				if(i == 128)
					i = 160;

				remapBuffer[0] = i;
				remapBuffer[1] = 0;

				if(MapANSI(remapBuffer,1,mapBuffer,3,NULL) == 1)
				{
					if(!(mapBuffer[1] & ~QUALIFIER_SHIFT))
					{
						event.ie_NextEvent			= NULL;
						event.ie_Class				= IECLASS_RAWKEY;
						event.ie_SubClass			= 0;
						event.ie_Code 				= mapBuffer[0];
						event.ie_Qualifier			= mapBuffer[1] & ~QUALIFIER_SHIFT;
						event.ie_position.ie_addr	= NULL;

						if(MapRawKey(&event,remapBuffer,10,NULL) == 1)
							LTP_Keys[0][i] = remapBuffer[0];

						event.ie_NextEvent			= NULL;
						event.ie_Class				= IECLASS_RAWKEY;
						event.ie_SubClass			= 0;
						event.ie_Code 				= mapBuffer[0];
						event.ie_Qualifier			= mapBuffer[1] | QUALIFIER_SHIFT;
						event.ie_position.ie_addr	= NULL;

						if(MapRawKey(&event,remapBuffer,10,NULL) == 1)
							LTP_Keys[1][i] = remapBuffer[0];
					}
				}
			}
		}

		ReleaseSemaphore(&LTP_KeySemaphore);
	}
	#endif	/* DO_PICKSHORTCUTS */

	UnlockThePubScreen = TRUE;
	WA_ScreenTag = WA_CustomScreen;

	if(!Screen)
	{
		if(PubScreen = (struct Screen *)GetTagData(LAHN_PubScreen,(IPTR)NULL,TagList))
		{
			UnlockThePubScreen = FALSE;
			WA_ScreenTag = WA_PubScreen;
			Screen = PubScreen;
		}
		else
		{
			STRPTR PubName;

			if(PubName = (STRPTR)GetTagData(LAHN_PubScreenName,(IPTR)NULL,TagList))
			{
				if(!(PubScreen = LockPubScreen(PubName)))
				{
					if(!GetTagData(LAHN_PubScreenFallBack,TRUE,TagList))
						return(NULL);
				}
			}
			else
				PubScreen = NULL;

			if(!PubScreen)
				PubScreen = LockPubScreen(NULL);

			if(!PubScreen)
				return(NULL);
			else
			{
				Screen = PubScreen;
				WA_ScreenTag = WA_PubScreen;
			}
		}
	}
	else
	{
		PubScreen = NULL;
		UnlockThePubScreen = FALSE;
	}

	if(Pool = AsmCreatePool(MEMF_PUBLIC | MEMF_ANY | MEMF_CLEAR,1024,1024,SysBase))
	{
		if(Handle = AsmAllocPooled(Pool,sizeof(LayoutHandle),SysBase))
		{
			struct TagItem	*List,
							*Entry;
			BOOL			 MenuGlyphs;

			MenuGlyphs = FALSE;

			Handle->Pool				= Pool;
			Handle->Screen				= Screen;
			Handle->PubScreen			= PubScreen;
			Handle->PointBack			= Handle;
			Handle->GroupID				= PHANTOM_GROUP_ID;

			Handle->WA_ScreenTag		= WA_ScreenTag;
			Handle->UnlockPubScreen		= UnlockThePubScreen;

			Handle->ExitFlush			= TRUE;
			Handle->RawKeyFilter		= TRUE;
			Handle->StandardEditHook	= &Handle->DefaultEditHook;

			#ifdef DO_CLONING
			{
				Handle->CloningPermitted	= TRUE;
			}
			#endif

			List = TagList;

			while(Entry = NextTagItem((const struct TagItem **)&List))
			{
				switch(Entry->ti_Tag)
				{
					case LH_Font:

						Handle->InitialTextAttr = (struct TextAttr *)Entry->ti_Data;
						break;

					case LH_MenuGlyphs:

						MenuGlyphs = Entry->ti_Data;
						break;

					case LH_Parent:

						Handle->Parent = (struct Window *)Entry->ti_Data;
						break;

					case LH_BlockParent:

						Handle->BlockParent = Entry->ti_Data;
						break;

					case LH_ExitFlush:

						Handle->ExitFlush = Entry->ti_Data;
						break;

					case LH_UserData:

						Handle->UserData = (APTR)Entry->ti_Data;
						break;

					case LH_RawKeyFilter:

						Handle->RawKeyFilter = Entry->ti_Data;
						break;

					case LH_AutoActivate:

						Handle->AutoActivate = Entry->ti_Data;
						break;

					case LH_LocaleHook:

						Handle->LocaleHook = (struct Hook *)Entry->ti_Data;
						break;

					case LH_EditHook:

						Handle->StandardEditHook = (struct Hook *)Entry->ti_Data;
						break;

					case LAHN_NoKeys:

						Handle->NoKeys = Entry->ti_Data;
						break;

					#ifdef DO_CLONING
					{
						case LH_CloningPermitted:

							Handle->CloningPermitted = Entry->ti_Data;
							break;

						case LH_SimpleClone:

							if(Handle->SimpleClone = Entry->ti_Data)
								Handle->ExactClone = FALSE;

							break;

						case LH_ExactClone:

							if(Handle->ExactClone = Entry->ti_Data)
								Handle->SimpleClone = FALSE;

							break;

						case LAHN_CloneScreenTitle:

							Handle->CloneScreenTitle = (STRPTR)Entry->ti_Data;
							break;
					}
					#endif

					case LAHN_TopGroupType:

						if(Entry->ti_Data == HORIZONTAL_KIND ||
						   Entry->ti_Data == VERTICAL_KIND)
						{
							TopGroupType = (LONG)Entry->ti_Data;
						}
						
						break;
				}
			}

			#ifdef DO_CLONING
			{
				if(Handle->LocaleHook)
				{
					if(Entry = FindTagItem(LAHN_CloneScreenTitle,TagList))
						Handle->CloneScreenTitle = (STRPTR)CallHookPkt(Handle->LocaleHook,Handle,(APTR)Entry->ti_Data);
				}
			}
			#endif

			#ifdef DO_PASSWORD_KIND
			{
				Handle->PasswordEditHook.h_Entry	= (HOOKFUNC)LTP_PasswordEditRoutine;
				Handle->PasswordEditHook.h_Data		= Handle;
			}
			#endif

			Handle->DefaultEditHook.h_Entry	= (HOOKFUNC)LTP_DefaultEditRoutine;
			Handle->DefaultEditHook.h_Data	= Handle;

			Handle->BackfillHook.h_Entry	= (HOOKFUNC)LTP_BackfillRoutine;
			Handle->BackfillHook.h_Data 	= Handle;

			#ifdef DO_HEXHOOK
			{
				Handle->HexEditHook.h_Entry 	= (HOOKFUNC)LTP_HexEditRoutine;
				Handle->HexEditHook.h_Data		= Handle;
			}
			#endif

			if(Handle->DrawInfo = GetScreenDrawInfo(Screen))
			{
				LONG i;

				Handle->TextPen			= Handle->DrawInfo->dri_Pens[TEXTPEN];
				Handle->BackgroundPen	= Handle->DrawInfo->dri_Pens[BACKGROUNDPEN];
				Handle->ShinePen		= Handle->DrawInfo->dri_Pens[SHINEPEN];
				Handle->ShadowPen		= Handle->DrawInfo->dri_Pens[SHADOWPEN];
				Handle->AspectX			= Handle->DrawInfo->dri_Resolution.X;
				Handle->AspectY			= Handle->DrawInfo->dri_Resolution.Y;

				if(Handle->BackgroundPen)
					InitRastPort(&Handle->BackfillRastPort);

				if(V39 && MenuGlyphs)
				{
					LONG i,Size,Width,Height = Handle->DrawInfo->dri_Font->tf_Baseline + 2;
					struct Image **Glyphs[2];

					Glyphs[0] = &Handle->AmigaGlyph;
					Glyphs[1] = &Handle->CheckGlyph;

					if(Screen->Flags & SCREENHIRES)
						Size = SYSISIZE_MEDRES;
					else
						Size = SYSISIZE_LOWRES;

					for(i = 0 ; i < 2 ; i++)
					{
						if(i)
							Width = (Height * Handle->AspectY) / Handle->AspectX;
						else
							Width = (Height * 3 * Handle->AspectY) / (2 * Handle->AspectX);

						if(!(*Glyphs[i] = NewObject(NULL,SYSICLASS,
							SYSIA_DrawInfo, Handle->DrawInfo,
							SYSIA_Size,		Size,
							SYSIA_Which,	i ? MENUCHECK : AMIGAKEY,
							IA_Width,		Width,
							IA_Height,		Height,
						TAG_DONE)))
						{
							DisposeObject(*Glyphs[0]);
							*Glyphs[0] = NULL;

							break;
						}
					}
				}

				for(i = 0 ; i < Handle->DrawInfo->dri_NumPens ; i++)
				{
					if(Handle->DrawInfo->dri_Pens[i] > Handle->MaxPen)
						Handle->MaxPen = Handle->DrawInfo->dri_Pens[i];
				}

				if(V39)
				{
					STATIC const ULONG DefaultItems[] =
					{
						IA_Width,		10,
						IA_Height,		10,
						IA_EdgesOnly,	TRUE,
						IA_Recessed,	TRUE,

						TAG_DONE
					};

					Handle->BevelImage = (struct Image *)NewObject(NULL,FRAMEICLASS,
						IA_FrameType,	FRAME_BUTTON,
					TAG_MORE,DefaultItems);

					Handle->GrooveImage = (struct Image *)NewObject(NULL,FRAMEICLASS,
						IA_FrameType,	FRAME_RIDGE,
					TAG_MORE,DefaultItems);

						/* Check if the class has been replaced, assuming that the
						 * replacement will live in memory rather than in ROM space.
						 */

					if(Handle->GrooveImage)
					{
						if(TypeOfMem(OCLASS(Handle->GrooveImage)->cl_Dispatcher.h_Entry))
							Handle->UseGroove = TRUE;
					}
				}

				if(Handle->VisualInfo = GetVisualInfoA(Screen,NULL))
				{
					InitRastPort(&Handle->RPort);

					if(LTP_GlyphSetup(Handle,Handle->InitialTextAttr))
					{
						#ifdef DO_PICKSHORTCUTS
						{
							memset(Handle->Keys,TRUE,256);

							for(i = 0 ; i < 256 ; i++)
							{
								if(i != 32 && i != 160)
								{
									if(LTP_Keys[0][i])
										Handle->Keys[LTP_Keys[0][i]] = FALSE;

									if(LTP_Keys[1][i])
										Handle->Keys[LTP_Keys[1][i]] = FALSE;
								}
							}
						}
						#endif

						if(TopGroupType != -1)
						{
							LT_AddA(Handle,TopGroupType,NULL,-1,NULL);

							Handle->CloseTopGroup = TRUE;
						}

						return(Handle);
					}
				}
			}

			LT_DeleteHandle(Handle);
		}
		else
			AsmDeletePool(Pool,SysBase);
	}
	else
		UnlockPubScreen(NULL,PubScreen);

	return(NULL);
}
