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

#include <stdarg.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

/****** gtlayout.library/LT_BuildA ******************************************
*
*   NAME
*	LT_BuildA -- turn the user interface specs into a window
*	             and gadgets.
*
*   SYNOPSIS
*	Window = LT_BuildA(Handle,Tags);
*	  D0                 A0    A1
*
*	struct Window *LT_BuildA(LayoutHandle *,struct TagItem *);
*
*	Window = LT_Build(Handle,...);
*
*	struct Window *LT_Build(LayoutHandle *,...);
*
*   FUNCTION
*	This is the big one. After building up a user interface specification
*	using LT_NewA() a call to LT_BuildA() will finally lay out the single
*	user interface elements, open a window and put the gadgets, etc.
*	inside.
*
*	The code tries to fit all the gadgets into the window, but if it
*	runs out of space it will fall back to a different font and
*	rescale the user interface objects to match it. It will first
*	fall back onto the system default font. If unsuccessful it will
*	as a last resort try to use topaz.font/8.
*
*	To make it easier to distinguish between different handles that
*	share the same Window->UserPort, the Window->UserData pointer
*	will point to the LayoutHandle that created it (V13).
*
*	    NOTE: Earlier library releases did not support this feature,
*	        so be prepared to deal with Window->UserData == NULL.
*
*   INPUTS
*	Handle - Pointer to a LayoutHandle structure.
*
*	Tags - Pointer to TagItem list controlling window
*	    and layout attributes.
*
*
*	All the tag values given are passed straight away to OpenWindowTags(),
*	see intuition.doc for more information.
*
*	In addition to this a number of private tag values are supported:
*
*	LAWN_Menu (struct Menu *) - The menu to attach to the window. The
*	    IDCMP flags will be updated to include IDCMP_MENUPICK if this
*	    tag is used.
*
*	LAWN_MenuTemplate (struct NewMenu *) - A list of filled-in
*	    NewMenu structures which will get passed straight through
*	    to LT_NewMenuTemplate(). If a menu could be created, it will
*	    be attached to the window. LT_DeleteHandle() will then later
*	    automatically dispose of the menu. Please note that the window
*	    may fail to open due to the menu layout going wrong. Separate
*	    calls to LT_NewMenuTemplate() and LT_Build() may be a better
*	    approach since it is easier to find out which process went
*	    wrong. You will find a pointer to the menu attached to the
*	    LayoutHandle in LayoutHandle->Menu. Please note that this
*	    entry only exists in LayoutHandles created by gtlayout.library
*	    v13 or higher. (V13)
*
*	        NOTE: This tag effectively overrides LAWN_Menu.
*
*	LAWN_MenuTags (struct TagItem *) - A list of TagItems which
*	    will get passed straight through to LT_NewMenuTagList().
*	    Even if you don't ask for it, LT_Build() will pass
*	    "LAMN_Handle,<Handle>" in for you, so any additional tags
*	    specifying screen, fonts, etc. will be overridden. If a menu
*	    could be created, it will be attached to the window.
*	    LT_DeleteHandle() will then later automatically dispose of the
*	    menu. Please note that the window may fail to open due to the
*	    menu layout going wrong. Separate calls to LT_NewMenuTagList()
*	    and LT_Build() may be a better approach since it is easier to
*	    find out which process went wrong. You will find a pointer to the
*	    menu attached to the LayoutHandle in LayoutHandle->Menu. Please note
*	    that this entry only exists in LayoutHandles created by
*	    gtlayout.library v13 or higher. (V13)
*
*	        NOTE: This tag effectively overrides LAWN_Menu and has
*	              precedence over LAWN_MenuTemplate.
*
*	LAWN_UserPort (struct MsgPort *) - The MsgPort to use as the
*	    window user port. The MsgPort will be attached using the
*	    common ModifyIDCMP() method, closing the window will
*	    first remove and reply all pending messages at this port.
*
*	LAWN_Left (LONG) - The left edge position the window is to use.
*	    This effectively overrides any horizontal alignment flags.
*
*	        NOTE: the code may choose to ignore this value if it finds
*	            that the window will not fit onto the screen unless
*	            the left edge position is changed.
*
*	LAWN_Top (LONG) - The top edge position the window is to use.
*	    This effectively overrides any vertical alignment flags.
*
*	        NOTE: the code may choose to ignore this value if it finds
*	            that the window will not fit onto the screen unless
*	            the top edge position is changed.
*
*	LAWN_Zoom (BOOL) - Adds a zoom gadget to the window. Clicking
*	    on this gadget will cause the window to shrink/zoom back
*	    to its original position. This differs from the WA_Zoom
*	    tag behaviour. When the window zooms back to its original
*	    position the gadgets are automatically refreshed.
*	    Default: FALSE
*
*	LAWN_MaxPen (LONG) - The maximum rendering pen index your code
*	    will use. Since you are -- with some restrictions -- allowed
*	    to render into the window created you may want to avoid
*	    silly side effects if drawing images or other colourful
*	    textures which do not share the common user interface colours.
*	    By default the layout code will change the maximum rendering
*	    pen number for the window to include only the user interface
*	    pen colours. This can, but need not disturb your own private
*	    window rendering.
*	    Look up graphics.library/SetMaxPen for more information.
*	    Default: determined by looking up Screen->DrawInfo.dri_Pens
*
*	LAWN_BelowMouse (BOOL) - This instructs the layout routine to
*	    centre the window created -- if possible -- below the
*	    mouse pointer. This effectively ignores any left edge,
*	    top edge or alignment settings.
*	    Default: FALSE
*
*	LAWN_MoveToWindow (BOOL) - When the window is finally open the
*	    user interface code will try to make sure the entire window
*	    is visible on the screen, this may involve moving the
*	    currently visible portion of an autoscrolling screen.
*	    Default: TRUE
*
*	LAWN_AutoRefresh (BOOL) - Handle IDCMP_REFRESHWINDOW events
*	    automatically.
*	    Default: TRUE
*
*	LAWN_HelpHook (struct Hook *) - Hook code to invoke when the user
*	    presses the "Help" key. See gtlayout.h for more information.
*	    Default: NULL
*
*	LAWN_Parent (struct Window *) - Parent window to centre the child
*	    window in.
*	    Default: NULL
*
*	LAWN_BlockParent (BOOL) - Lock the parent window via LT_LockWindow()
*	    until the child window is closed.
*
*	        NOTE: requires LAWN_Parent attribute.
*
*	    Default: FALSE
*
*	LAWN_SmartZoom (BOOL) - Attach a zoom gadget to the window created.
*	    When in zoomed state, the window will be as small as possible,
*	    showing only the window title and window gadgets:
*
*	        NOTE: this tag implies LAWN_Zoom,TRUE
*
*	    Default: FALSE
*
*	LAWN_Title (STRPTR) - The window title to use. Use this tag in
*	    place of WA_Title or you will break the layout code.
*	    Default: no title
*
*	LAWN_Bounds (struct IBox *) - Bounds to centre the window in.
*	    Default: NULL
*
*	LAWN_ExtraWidth (LONG) - Extra width to add into the calculation
*	    when opening the window.
*	    Default: 0
*
*	LAWN_ExtraHeight (LONG) - Extra height to add into the calculation
*	    when opening the window.
*	    Default: 0
*
*	LAWN_IDCMP (ULONG) - Use this tag in place of WA_IDCMP or you
*	    will break the object handling code.
*
*	LAWN_AlignWindow (UWORD) - Alignment information for the window, must
*	    be a mask made from the following bit values:
*
*	        ALIGNF_RIGHT - Align to screen right edge
*	        ALIGNF_LEFT - Align to screen left edge
*	        ALIGNF_TOP - Align to screen top edge
*	        ALIGNF_BOTTOM - Align to screen bottom edge
*
*	    Unless forbidden (such as by passing ALIGNF_RIGHT|ALIGNF_TOP)
*	    the window will be centered horizontally and/or vertically.
*
*	LAWN_FlushLeft (BOOL) - Add no horizontal space surrounding the
*	    objects the windows will hold. (V10)
*
*	LAWN_FlushTop (BOOL) - Add no vertical space surrounding the
*	    objects the windows will hold. (V10)
*
*	LAWN_Show (BOOL) - Make the window visible when it is opened;
*	    this may involve depth-arranging screens. (V10)
*           Default: FALSE
*
*	LAWN_NoInitialRefresh (BOOL) - If set to TRUE, adds the
*	    gadgets, but does not draw the window imagery. You need
*	    to draw them later by calling gtlayout.library/LT_Refresh.
*	    Default: FALSE
*
*	LAWN_LimitWidth (UWORD) - Limit the width of the window to this
*	    value. (V35)
*
*	LAWN_LimitHeight (UWORD) - Limit the height of the window to this
*	    value. (V35)
*
*	LAWN_UserData (APTR) - Store this pointer with the Window information.
*	    you can later retrieve it with the LT_GetWindowUserData()
*	    call (V39).
*
*   RESULT
*	Window - Pointer to a Window structure.
*
*   SEE ALSO
*       gtlayout.library/LT_Refresh
*       gtlayout.library/LT_GetWindowUserData
*       intuition.library/OpenWindow
*       intuition.library/OpenWindowTagList
*
******************************************************************************
*
*/

struct Window * LIBENT
LT_BuildA(REG(a0) LayoutHandle *handle,REG(a1) struct TagItem *TagParams)
{
	ULONG			 OpenWindowTag;
	LONG			 left, top, width, height;
	LONG			 OldLeft,OldTop,OldRight,OldBottom;
	struct IBox		 newBounds;
	struct Menu		*menu;
	struct TagItem	*item;
	LONG			 placeLeft;
	LONG			 placeTop;
	struct IBox		 zoomBox;
	struct IBox		*zoom;
	struct TextFont *font;
	struct TagItem	*NewTags,*Tags;
	struct Window	*Parent;
	STRPTR			 title;
	struct IBox		*bounds;
	LONG			 extraWidth,
					 extraHeight;
	ULONG			 IDCMP;
	LONG			 align;
	LONG			 MinX,
					 MinY,
					 MaxX,
					 MaxY;
	ULONG			 BorderBottom,
					 BorderRight;
	struct Menu		*LocalMenu;
	struct TagItem	*LocalMenuTags;
	struct NewMenu	*LocalMenuTemplate;
	BOOL			 SizeGadget,
					 SizeBRight,
					 SizeBBottom;
	BOOL		 	 BlockParent,
					 SmartZoom,
					 MakeVisible;
	BOOL			 DontRefresh;
	struct TagItem	*TagList;
	WORD			 WithinCounter;

	if(!handle)
		return(NULL);

	menu				= NULL;
	NewTags				= NULL;
	Parent				= handle->Parent;
	title				= NULL;
	bounds 				= NULL;
	extraWidth			= 0,
	extraHeight			= 0;
	IDCMP				= 0;
	align				= 0;
	LocalMenu			= NULL;
	LocalMenuTags		= NULL;
	LocalMenuTemplate	= NULL;
	SizeGadget			= FALSE,
	SizeBRight			= FALSE,
	SizeBBottom			= FALSE;
	BlockParent			= handle->BlockParent,
	SmartZoom			= FALSE,
	MakeVisible			= FALSE;
	DontRefresh			= FALSE;

	WithinCounter = 0;

	memset(&newBounds,0,sizeof(newBounds));	/* Initialize this for the sake of the compiler. */

	/* close the outermost layout group, if necessary (V45) */
	if(handle->CloseTopGroup)
	{
		LT_EndGroup(handle);

		handle->CloseTopGroup = FALSE;
	}

	TagList = (struct TagItem *)TagParams;
	while(item = NextTagItem((const struct TagItem **)&TagList))
	{
		switch(item->ti_Tag)
		{
			case LAWN_LimitWidth:

				WithinCounter |= 1;
				newBounds.Width = item->ti_Data;
				break;

			case LAWN_UserData:

				handle->WindowUserData = (APTR)item->ti_Data;
				break;

			case LAWN_LimitHeight:

				WithinCounter |= 2;
				newBounds.Height = item->ti_Data;
				break;

			case LAWN_NoInitialRefresh:

				DontRefresh = item->ti_Data;
				break;

			case LAWN_MenuTemplate:

				LocalMenuTemplate = (struct NewMenu *)item->ti_Data;
				break;

			case LAWN_MenuTags:

				LocalMenuTags = (struct TagItem *)item->ti_Data;
				break;

			case LAWN_FlushLeft:

				handle->FlushLeft = item->ti_Data;
				break;

			case LAWN_FlushTop:

				handle->FlushTop = item->ti_Data;
				break;

			case LAWN_TitleText:

				title = (STRPTR)item->ti_Data;
				break;

			case LAWN_TitleID:

				if(!handle->LocaleHook)
					return(NULL);
				else
					title = (STRPTR)CallHookPkt(handle->LocaleHook,handle,(APTR)item->ti_Data);

				break;

			case LAWN_Bounds:

				bounds = (struct IBox *)item->ti_Data;
				break;

			case LAWN_ExtraWidth:

				extraWidth = item->ti_Data;
				break;

			case LAWN_ExtraHeight:

				extraHeight = item->ti_Data;
				break;

			case LAWN_IDCMP:

				IDCMP = item->ti_Data;
				break;

			case LAWN_AlignWindow:

				align = item->ti_Data;
				break;
		}
	}

	if(!bounds && WithinCounter == 3)
	{
		UWORD width,height;

		width = newBounds.Width;
		height = newBounds.Height;

		LTP_GetDisplayClip(handle->Screen,&newBounds.Left,&newBounds.Top,&newBounds.Width,&newBounds.Height);
		bounds = &newBounds;

		bounds->Width = width;
		bounds->Height = height;
	}

	#ifdef DO_MENUS
	{
		if(LocalMenuTags)
		{
			if(LocalMenu = LT_NewMenuTags(LAMN_Handle,handle,TAG_MORE,LocalMenuTags))
			{
				menu = LocalMenu;
				handle->IDCMP |= IDCMP_MENUPICK;
			}
			else
				return(NULL);
		}
		else
		{
			if(LocalMenuTemplate)
			{
				if(LocalMenu = LT_NewMenuTemplate(handle->Screen,handle->TextAttr,handle->AmigaGlyph,handle->CheckGlyph,NULL,LocalMenuTemplate))
				{
					menu = LocalMenu;
					handle->IDCMP |= IDCMP_MENUPICK;
				}
				else
					return(NULL);
			}
		}
	}
	#endif	/* DO_MENUS */

	if(!bounds)
	{
		LTP_GetDisplayClip(handle->Screen,&newBounds.Left,&newBounds.Top,&newBounds.Width,&newBounds.Height);

		bounds = &newBounds;
	}

	left = handle->Screen->WBorLeft;

	if(title)
		top = handle->Screen->WBorTop + handle->Screen->Font->ta_YSize + 1;
	else
		top = handle->Screen->WBorTop;

	BorderRight		= handle->Screen->WBorRight;
	BorderBottom	= handle->Screen->WBorBottom;

	if(handle->ResizeObject != NULL)
	{
		BOOLEAN ResizeX,ResizeY;

		/* Note: ResizeObject can either be a LISTVIEW_KIND or a
		 *       FRAME_KIND!
		 */
		if(handle->ResizeObject->Type == LISTVIEW_KIND)
		{
			ResizeX = handle->ResizeObject->Special.List.ResizeX;
			ResizeY = handle->ResizeObject->Special.List.ResizeY;
		}
		else
		{
			ResizeX = handle->ResizeObject->Special.Frame.ResizeX;
			ResizeY = handle->ResizeObject->Special.Frame.ResizeY;
		}

		if(ResizeY)
		{
			BorderBottom = LTP_GetSizeHeight(handle);
		}
		else
		{
			if(ResizeX)
				BorderRight = LTP_GetSizeWidth(handle);
		}
	}

	OldLeft		= left;
	OldRight	= BorderRight;
	OldTop		= top;
	OldBottom	= BorderBottom;

	if(!handle->FlushLeft)
	{
		left		+= handle->InterWidth;
		BorderRight	+= handle->InterWidth;
	}

	if(!handle->FlushTop)
	{
		top				+= handle->InterHeight;
		BorderBottom	+= handle->InterHeight;
	}

	LTP_CreateGadgets(handle,bounds,left,top,left + BorderRight,top + BorderBottom);

	if(handle->Failed)
		return(NULL);

		// In case the font got changed we'll have to redo it all again.
		// Just to be sure, we do it all over again.

	left			= OldLeft;
	BorderRight		= OldRight;
	top				= OldTop;
	BorderBottom	= OldBottom;

	if(!handle->FlushLeft)
	{
		left		+= handle->InterWidth;
		BorderRight	+= handle->InterWidth;
	}

	if(!handle->FlushTop)
	{
		top				+= handle->InterHeight;
		BorderBottom	+= handle->InterHeight;
	}

	width	= left + handle->TopGroup->Width + BorderRight;
	height	= top + handle->TopGroup->Height + BorderBottom;

	if(handle->ResizeObject != NULL)
	{
		LONG GlyphWidth,GlyphHeight;
		BOOLEAN ResizeX,ResizeY;

		/* Note: ResizeObject can either be a LISTVIEW_KIND or a
		 *       FRAME_KIND!
		 */
		if(handle->ResizeObject->Type == LISTVIEW_KIND)
		{
			ResizeX = handle->ResizeObject->Special.List.ResizeX;
			ResizeY = handle->ResizeObject->Special.List.ResizeY;
		}
		else
		{
			ResizeX = handle->ResizeObject->Special.Frame.ResizeX;
			ResizeY = handle->ResizeObject->Special.Frame.ResizeY;
		}

		MaxX = MinX = width;
		MaxY = MinY = height;

		if(ResizeX)
			MaxX = handle->Screen->Width;

		if(ResizeY)
		{
			MaxY = handle->Screen->Height;

			SizeBBottom = TRUE;
		}
		else
		{
			SizeBRight = TRUE;
		}

		SizeGadget = TRUE;

		GlyphWidth	= handle->GlyphWidth;
		GlyphHeight	= handle->GlyphHeight;

		if(handle->ResizeObject->Type == LISTVIEW_KIND)
		{
			if(handle->ResizeObject->Special.List.TextAttr != NULL)
			{
				GlyphWidth	= handle->ResizeObject->Special.List.FixedGlyphWidth;
				GlyphHeight	= handle->ResizeObject->Special.List.FixedGlyphHeight;
			}
		
			if(handle->ResizeObject->Special.List.MinChars && handle->ResizeObject->Special.List.MinChars < handle->ResizeObject->Chars)
				MinX -= GlyphWidth * (handle->ResizeObject->Chars - handle->ResizeObject->Special.List.MinChars);

			if(handle->ResizeObject->Special.List.MinLines && handle->ResizeObject->Special.List.MinChars < handle->ResizeObject->Lines)
				MinY -= GlyphHeight * (handle->ResizeObject->Lines - handle->ResizeObject->Special.List.MinLines);
		}
		else
		{
			if(MinX < handle->ResizeObject->Special.Frame.InnerWidth)
				MinX = handle->ResizeObject->Special.Frame.InnerWidth;

			if(MinY < handle->ResizeObject->Special.Frame.InnerHeight)
				MinY = handle->ResizeObject->Special.Frame.InnerHeight;
		}
	}
	else
	{
		MaxX = MinX = width;
		MaxY = MinY = height;
	}

	if(align & ALIGNF_LEFT)
		placeLeft = 0;
	else
	{
		if(align & ALIGNF_RIGHT)
			placeLeft = bounds->Width - (width + extraWidth);
		else
			placeLeft = (bounds->Width - (width + extraWidth)) / 2;
	}

	if(align & ALIGNF_TOP)
		placeTop = 0;
	else
	{
		if(align & ALIGNF_BOTTOM)
			placeTop = bounds->Height - (height + extraHeight);
		else
			placeTop = (bounds->Height - (height + extraHeight)) / 2;
	}

	if(align & ALIGNF_EXTRA_LEFT)
		placeLeft += extraWidth;
	else
	{
		if(!(align & ALIGNF_EXTRA_RIGHT))
			placeLeft += extraWidth / 2;
	}

	if(align & ALIGNF_EXTRA_TOP)
		placeTop += extraHeight;
	else
	{
		if(!(align & ALIGNF_EXTRA_BOTTOM))
			placeTop += extraHeight / 2;
	}

	placeLeft	+= bounds->Left;
	placeTop	+= bounds->Top;

	if(placeLeft < 0)
		placeLeft = 0;

	if(placeTop < 0)
		placeTop = 0;

	zoom = NULL;

	handle->AutoRefresh = TRUE;

	#ifdef DO_CLONING
	{
		if(handle->CloneExtra)
		{
			placeLeft	= 0;
			placeTop	= handle->CloneExtra->Screen->BarHeight + 1;

			zoomBox.Left	= placeLeft;
			zoomBox.Top		= placeTop;
		}
	}
	#endif	/* DO_CLONING */

	TagList = (struct TagItem *)TagParams;

	while(item = NextTagItem((const struct TagItem **)&TagList))
	{
		switch((ULONG)item->ti_Tag)
		{
			case LA_Menu:

				if(!menu)
				{
					handle->IDCMP |= IDCMP_MENUPICK;

					menu = (struct Menu *)item->ti_Data;
				}

				break;

			case LAWN_SmartZoom:

				if(!(SmartZoom = item->ti_Data))
					break;

			case LAWN_Zoom:

				if(item->ti_Data)
				{
					if(title && SmartZoom)
					{
						STATIC const UWORD WhichTable[3] =
						{
							CLOSEIMAGE,
							ZOOMIMAGE,
							DEPTHIMAGE
						};

						LONG	 Size = 0,i;
						LONG	 SizeType;
						Object	*Image;
						BOOL	 GotIt = TRUE;

						if(handle->Screen->Flags & SCREENHIRES)
							SizeType = SYSISIZE_MEDRES;
						else
							SizeType = SYSISIZE_LOWRES;

						for(i = 0 ; i < 3 ; i++)
						{
							if(Image = NewObject(NULL,SYSICLASS,
								SYSIA_Size,		SizeType,
								SYSIA_Which,	WhichTable[i],
								SYSIA_DrawInfo, handle->DrawInfo,
							TAG_DONE))
							{
								IPTR Width;

								GetAttr(IA_Width,Image,&Width);

								Size += Width;

								DisposeObject(Image);
							}
							else
								GotIt = FALSE;
						}

						if(GotIt)
						{
							Size += 8 + TextLength(&handle->Screen->RastPort,title,strlen(title)) + 8;

							zoomBox.Width = Size;
						}
						else
							zoomBox.Width = width;
					}
					else
						zoomBox.Width = width;

					if(V39)
					{
						zoomBox.Left	= -1;
						zoomBox.Top		= -1;
					}
					else
					{
						zoomBox.Left	= placeLeft;
						zoomBox.Top		= placeTop;
					}

					zoomBox.Height = handle->Screen->WBorTop + handle->Screen->Font->ta_YSize + 1;

					zoom = &zoomBox;
				}

				break;

			case LAWN_UserPort:

				handle->MsgPort = (struct MsgPort *)item->ti_Data;
				break;

			case LAWN_HelpHook:

				handle->HelpHook = (struct Hook *)item->ti_Data;
				break;

			case LAWN_Parent:

				Parent = (struct Window *)item->ti_Data;
				break;

			case LAWN_BlockParent:

				BlockParent = item->ti_Data;
				break;

			case LAWN_BelowMouse:

				if((placeLeft = handle->Screen->MouseX - (width / 2)) < 0)
					placeLeft = 0;

				if((placeTop = handle->Screen->MouseY - (height / 2)) < 0)
					placeTop = 0;

				break;

			case LAWN_MaxPen:

				handle->MaxPen = (LONG)item->ti_Data;
				break;

			case LAWN_MoveToWindow:

				handle->MoveToWindow = (LONG)item->ti_Data;
				break;

			case LAWN_AutoRefresh:

				handle->AutoRefresh = (LONG)item->ti_Data;
				break;

			case LAWN_Show:

				MakeVisible = item->ti_Data;
				break;
		}
	}

	Tags = (struct TagItem *)TagParams;

	if(handle->BackgroundPen && !V39)
	{
		if(NewTags = CloneTagItems(Tags))
		{
			STATIC const Tag Filter[] = { WA_SimpleRefresh,TAG_DONE };

			FilterTagItems(NewTags,(Tag *)Filter,TAGFILTER_NOT);

			Tags = NewTags;
		}
		else
		{
			#ifdef DO_MENUS
			{
				LT_DisposeMenu(LocalMenu);
			}
			#endif	/* DO_MENUS */

			return(NULL);
		}
	}

	if(Parent)
	{
		WORD	Left,Top,Width,Height;
		LONG	MoveLeft,MoveTop,WindowLeft,WindowTop,WindowWidth,WindowHeight;

		WindowLeft		= Parent->LeftEdge + Parent->BorderLeft;
		WindowTop		= Parent->TopEdge + Parent->BorderTop;
		WindowWidth		= Parent->Width - (Parent->BorderLeft + Parent->BorderRight);
		WindowHeight	= Parent->Height - (Parent->BorderTop + Parent->BorderBottom);

		LTP_GetDisplayClip(Parent->WScreen,&Left,&Top,&Width,&Height);

		if((MoveLeft = WindowLeft + (WindowWidth - width) / 2) < 0)
			MoveLeft = 0;

		if((MoveTop = WindowTop + (WindowHeight - height) / 2) < 0)
			MoveTop = 0;

		if(MoveLeft < Left || MoveLeft + width > Left + Width)
			MoveLeft = -1;

		if(MoveTop < Top || MoveTop + height > Top + Height)
			MoveTop = -1;

		if(MoveTop != -1 && MoveLeft != -1)
		{
			placeLeft	= MoveLeft;
			placeTop	= MoveTop;
		}
	}

	TagList = (struct TagItem *)TagParams;

	while(item = NextTagItem((const struct TagItem **)&TagList))
	{
		switch((ULONG)item->ti_Tag)
		{
			case LAWN_Left:

				placeLeft = item->ti_Data;
				break;

			case LAWN_Top:

				placeTop = item->ti_Data;
				break;
		}
	}

	if(BlockParent && Parent)
	{
		LT_LockWindow(Parent);

		handle->Parent = Parent;
	}

	if(SizeGadget)
		zoom = NULL;

	if(handle->Screen == handle->PubScreen)
		OpenWindowTag = handle->WA_ScreenTag;
	else
		OpenWindowTag = WA_CustomScreen;

	if(handle->Window = OpenWindowTags(NULL,
		WA_Left,			placeLeft,
		WA_Top, 			placeTop,
		WA_Width,			width,
		WA_Height,			height,
		WA_NewLookMenus,	TRUE,
		OpenWindowTag,		handle->Screen,
		WA_MinWidth,		MinX,
		WA_MinHeight,		MinY,
		WA_MaxWidth,		MaxX,
		WA_MaxHeight,		MaxY,
		WA_SizeGadget,		SizeGadget,
		WA_SizeBBottom,		SizeBBottom,
		WA_SizeBRight,		SizeBRight,
		WA_PointerDelay,	TRUE,
		WA_BusyPointer,		TRUE,

		zoom ? WA_Zoom : TAG_IGNORE,						zoom,
		!handle->MsgPort ? WA_IDCMP : TAG_IGNORE,			IDCMP_REFRESHWINDOW | IDCMP_RAWKEY | IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_CHANGEWINDOW | IDCMP | handle->IDCMP,
		title ? WA_Title : TAG_IGNORE,						title,
		handle->AmigaGlyph ? WA_AmigaKey : TAG_IGNORE,		handle->AmigaGlyph,
		handle->CheckGlyph ? WA_Checkmark : TAG_IGNORE,		handle->CheckGlyph,
		handle->BackgroundPen ? WA_BackFill : TAG_IGNORE,	&handle->BackfillHook,
	TAG_MORE,Tags))
	{
		font = handle->RPort.Font;

		handle->RPort = *handle->Window->RPort;

		LTP_SetFont(handle,font);

		if(V39 && handle->MaxPen > 0)
		{
			SetMaxPen(&handle->RPort,handle->MaxPen);
			SetMaxPen(handle->Window->RPort,handle->MaxPen);
		}

		if(NewTags)
		{
			FreeTagItems(NewTags);
			NewTags = NULL;
		}

			// It's really that simple...

		if(DontRefresh)
			LTP_AddGadgetsDontRefresh(handle);
		else
			LTP_AddGadgets(handle);

		handle->Window->UserData = (APTR)handle;

		if(handle->MsgPort)
		{
			handle->Window->UserPort = handle->MsgPort;

			if(!ModifyIDCMP(handle->Window,IDCMP_CHANGEWINDOW | IDCMP_REFRESHWINDOW | IDCMP_RAWKEY | IDCMP_INACTIVEWINDOW | IDCMP_ACTIVEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP | handle->IDCMP))
			{
				handle->Window->UserPort = NULL;

				return (NULL);
			}
		}

		if(menu)
			SetMenuStrip(handle->Window,menu);

		handle->Menu = LocalMenu;

		LTP_SelectInitialActiveGadget(handle);

		#ifdef DO_CLONING
		{
			if(handle->CloneExtra)
				ScreenToFront(handle->Window->WScreen);
		}
		#endif

		if(V39)
			SetWindowPointerA(handle->Window,NULL);

		if(MakeVisible)
			ScreenToFront(handle->Screen);

		if(handle->MoveToWindow || MakeVisible)
			LTP_MoveToWindow(handle);

		if((handle->Window->Flags & WFLG_WINDOWACTIVE) && handle->ActiveString && !DontRefresh)
			LT_Activate(handle,handle->ActiveString->ID);
	}
	else
	{
		if(handle->Parent)
		{
			LT_UnlockWindow(handle->Parent);

			handle->Parent = NULL;
		}

		#ifdef DO_MENUS
		{
			LT_DisposeMenu(LocalMenu);
		}
		#endif	/* DO_MENUS */
	}

	FreeTagItems(NewTags);

	return(handle->Window);
}


/*****************************************************************************/


VOID
LTP_SelectInitialActiveGadget(LayoutHandle *Handle)
{
	ObjectNode *Node;
	LONG i;

	Handle->Previous = NULL;

	for(i = 0 ; i < Handle->Count ; i++)
	{
		if(Handle->GadgetArray[i])
		{
			if(GETOBJECT(Handle->GadgetArray[i],Node))
			{
				if(LIKE_STRING_KIND(Node) || (Node->Type == INTEGER_KIND))
				{
					Handle->Previous = Handle->GadgetArray[i];
					break;
				}
			}
		}
	}
}


/*****************************************************************************/

#ifndef __AROS__
struct Window *
LT_Layout(LayoutHandle *handle,STRPTR title,struct IBox *bounds,LONG extraWidth,LONG extraHeight,ULONG IDCMP,LONG align,...)
{
	struct Window *Result;
	va_list VarArgs;

	va_start(VarArgs,align);

	Result = LT_Build(handle,
		LAWN_Title,			title,
		LAWN_Bounds,		bounds,
		LAWN_ExtraWidth,	extraWidth,
		LAWN_ExtraHeight,	extraHeight,
		LAWN_IDCMP,			IDCMP,
		LAWN_AlignWindow,	align,
	TAG_MORE,(struct TagItem *)VarArgs);

	va_end(VarArgs);

	return(Result);
}
#endif

/*****************************************************************************/


struct Window * LIBENT
LT_LayoutA(REG(a0) LayoutHandle *handle,REG(a1) STRPTR title,REG(a2) struct IBox *bounds,REG(d0) LONG extraWidth,REG(d1) LONG extraHeight,REG(d2) ULONG IDCMP,REG(d3) LONG align,REG(a3) struct TagItem *TagParams)
{
	struct Window *Result;

	Result = LT_Build(handle,
		LAWN_Title,			title,
		LAWN_Bounds,		bounds,
		LAWN_ExtraWidth,	extraWidth,
		LAWN_ExtraHeight,	extraHeight,
		LAWN_IDCMP,			IDCMP,
		LAWN_AlignWindow,	align,
	TAG_MORE,TagParams);

	return(Result);
}


/*****************************************************************************/


#ifndef __AROS__
struct Window *
LT_Build(LayoutHandle *Handle,...)
{
	struct Window *Result;
	va_list VarArgs;

	va_start(VarArgs,Handle);
	Result = LT_BuildA(Handle,(struct TagItem *)VarArgs);
	va_end(VarArgs);

	return(Result);
}
#endif


/*****************************************************************************/


/****** gtlayout.library/LT_GetWindowUserData *******************************
*
*   NAME
*	LT_GetWindowUserData -- Obtain user data information associated
*	    with a Window created by gtlayout.library/LT_Build (V39).
*
*   SYNOPSIS
*	Data = LT_GetWindowUserData(Window,DefaultValue);
*	  D0                         A0        A1
*
*	APTR LT_GetWindowUserData(struct Window *,APTR);
*
*   FUNCTION
*	You can tell the build process to store user data with the Window
*	it creates. This can be used as a replacement for the Window->UserData
*	pointer information which gtlayout.library uses for itself. If the
*	Window you pass to this routine was created by gtlayout.library/LT_Build
*	you will receive the user data pointer you passed in, otherwise the
*	given default value will be returned.
*
*   INPUTS
*	Window - Pointer to a Window created by gtlayout.library/LT_Build
*
*   RESULT
*	Data - The user data pointer you provided for gtlayout.library/LT_Build
*	    with the LAWN_UserData tag or the default value if no such pointer
*	    was provided or this Window was not created by
*	    gtlayout.library/LT_Build.
*
*	DefaultValue - If the window was not created by gtlayout.library/LT_Build
*	    this is the value you will receive as the function result.
*
*   SEE ALSO
*       gtlayout.library/LT_Build
*
******************************************************************************
*
*/

APTR LIBENT
LT_GetWindowUserData(REG(a0) struct Window *Window,REG(a1) APTR DefaultValue)
{
	APTR Result;

	Result = DefaultValue;

	if(Window != NULL)
	{
		if(Window->UserData != NULL && !((IPTR)Window->UserData & 1))
		{
			LayoutHandle *Local = (LayoutHandle *)Window->UserData;

			if(Local->PointBack == Local)
				Result = Local->WindowUserData;
		}
	}

	return(Result);
}
