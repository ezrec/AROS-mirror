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

#ifndef __AROS__
struct Menu *
LT_LayoutMenus(LayoutHandle *handle,struct NewMenu *menuTemplate,...)
{
	struct Menu	*Result;
	va_list 	 VarArgs;

	va_start(VarArgs,menuTemplate);
	Result = LT_LayoutMenusA(handle,menuTemplate,(struct TagItem *)VarArgs);
	va_end(VarArgs);

	return(Result);
}
#endif


/*****************************************************************************/


/****** gtlayout.library/LT_LayoutMenusA ******************************************
*
*   NAME
*	LT_LayoutMenusA -- Create a menu according to a template.
*
*   SYNOPSIS
*	Menu = LT_LayoutMenusA(Handle,Template,Tags);
*	 D0                      A0     A1      A2
*
*	struct Menu *LT_LayoutMenusA(LayoutHandle *,struct NewMenu *,
*
*	    struct TagItem *);
*
*	Menu = LT_LayoutMenus(Handle,Template,...);
*
*	struct Menu *LT_LayoutMenus(LayoutHandle *,struct NewMenu *,...);
*
*   FUNCTION
*	Unlike the corresponding routines in gadtools.library
*	LT_LayoutMenusA() will both create and layout a menu.
*	Also included is locale support.
*
*   INPUTS
*	Handle - Pointer to LayoutHandle structure.
*
*	Template - Address of a ready-to-use NewMenu table to
*	    create the menu from.
*
*	Tags - Tagitem list to control menu attributes
*
*	Tags:
*
*	LAMN_FirstLabel (LONG) - Locale ID of the first string to
*	    use as a menu title/item/subitem label. This tag
*	    works in conjunction with LA_LastLabel.
*
*	LAMN_LastLabel (LONG) - Locale ID of the last string to
*	    use as a menu title/item/subitem label. This tag
*	    works in conjunction with LA_FirstLabel. The code
*	    will loop through FirstLabel..LastLabel and assign
*	    the corresponding locale text for each ID to the
*	    NewMenu.nm_Label entries. Labels which are already
*	    initialized with NM_BARLABEL are skipped.
*
*	LAMN_LabelTable (LONG *) - Pointer to an array of IDs
*	    to use for building the menu labels. This requires
*	    that a locale hook is provided with the layout handle.
*	    The array is terminated by -1.
*
*   RESULT
*	Menu - Pointer to a Menu structure. You can free this
*	       using gadtools.library/FreeMenus().
*
******************************************************************************
*
*/

struct Menu * LIBENT
LT_LayoutMenusA(REG(a0) LayoutHandle *handle,REG(a1) struct NewMenu *menuTemplate,REG(a2) struct TagItem *TagParams)
{
	struct Menu *menu;

	menu = NULL;

	if(handle)
	{
		struct TagItem	*tag,*List = TagParams;
		LONG			 label = 0,last = 0;	/* For the sake of the compiler, initialize these. */
		LONG			*Table = NULL;
		ULONG			 allocSize = 0;	/* For the sake of the compiler, initialize this. */
		BOOL	 		 foundFirst;
		BOOL	 		 foundLast;

		foundFirst	= FALSE;
		foundLast	= FALSE;

		while(tag = NextTagItem(&List))
		{
			switch(tag->ti_Tag)
			{
				case LA_LabelTable:

					Table = (LONG *)tag->ti_Data;
					break;

				case LA_FirstLabel:

					foundFirst = TRUE;

					label = tag->ti_Data;
					break;

				case LA_LastLabel:

					foundLast = TRUE;

					last = tag->ti_Data;
					break;
			}
		}

		if(foundFirst == foundLast || Table)
		{
			struct NewMenu *localTemplate = NULL;

			if(Table)
			{
				foundFirst = FALSE;

				if(handle->LocaleHook)
				{
					LONG count;

					count = 0;

					while(menuTemplate[count].nm_Type != NM_END)
						count++;

					count++;

					if(localTemplate = LTP_Alloc(handle,allocSize = sizeof(struct NewMenu) * count))
					{
						LONG i;

						CopyMem(menuTemplate,localTemplate,sizeof(struct NewMenu) * count);

						for(i = 0 ; *Table != -1 && i < count ; i++)
						{
							if(localTemplate[i].nm_Label != NM_BARLABEL && (localTemplate[i].nm_Type == NM_TITLE || localTemplate[i].nm_Type == NM_ITEM || localTemplate[i].nm_Type == NM_SUB))
								localTemplate[i].nm_Label = (STRPTR)CallHookPkt(handle->LocaleHook,handle,(APTR)(*Table++));
						}
					}
				}

				menuTemplate = localTemplate;
			}

			if(foundFirst)
			{
				if(handle->LocaleHook)
				{
					LONG count;

					count = 0;

					while(menuTemplate[count].nm_Type != NM_END)
						count++;

					count++;

					if(localTemplate = LTP_Alloc(handle,allocSize = sizeof(struct NewMenu) * count))
					{
						LONG i;

						CopyMem(menuTemplate,localTemplate,sizeof(struct NewMenu) * count);

						if(foundLast)
							count = last - label + 1;

						for(i = 0 ; i < count ; i++)
						{
							if(localTemplate[i].nm_Label != NM_BARLABEL && (localTemplate[i].nm_Type == NM_TITLE || localTemplate[i].nm_Type == NM_ITEM || localTemplate[i].nm_Type == NM_SUB))
							{
								localTemplate[i].nm_Label = (STRPTR)CallHookPkt(handle->LocaleHook,handle,(APTR)label);

								label++;
							}
						}
					}
				}

				menuTemplate = localTemplate;
			}

			if(menuTemplate)
			{
				if(menu = CreateMenusA(menuTemplate,NULL))
				{
					if(!LayoutMenus(menu,handle->VisualInfo,
						GTMN_NewLookMenus,	TRUE,

						handle->AmigaGlyph ? GTMN_AmigaKey : TAG_IGNORE,handle->AmigaGlyph,
						handle->CheckGlyph ? GTMN_Checkmark : TAG_IGNORE,handle->CheckGlyph,
					TAG_DONE))
					{
						FreeMenus(menu);

						menu = NULL;
					}
				}
			}

			LTP_Free(handle,localTemplate,allocSize);
		}
	}

	return(menu);
}
