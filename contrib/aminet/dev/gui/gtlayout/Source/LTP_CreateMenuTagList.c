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

#include <dos/dos.h>	/* For AmigaDOS error definitions */

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

#ifdef DO_MENUS	/* Support code */

	/* LTP_CreateMenuTagList(RootMenu *Root,LONG *Error,struct TagItem *TagList):
	 *
	 *	Create the menu strip data.
	 */

BOOL
LTP_CreateMenuTagList(RootMenu *Root,LONG *ErrorPtr,struct TagItem *TagList)
{
	MenuNode		*LastMenu	= NULL;
	ItemNode		*LastItem	= NULL;
	ItemNode		*LastSub	= NULL;

	MenuNode		*Menu;
	ItemNode		*Item;
	ItemNode		*Sub;

	struct TagItem	*List = TagList,*Entry;

	struct NewMenu	 Template;

	LONG			 Char		= 0;
	LONG			 Code		= 0;
	ULONG			 Qualifier	= 0;
	ULONG			 ID			= 0;

	LONG			 Error		= 0;

	BOOL			 Starting	= TRUE;

	memset(&Template,0,sizeof(Template));	/* For the sake of the compiler, make sure this is initialized */

	for(;;)
	{
		Entry = NextTagItem((const struct TagItem **)&List);

		if(!Entry || (Entry && Entry->ti_Tag >= LAMN_TitleText && Entry->ti_Tag <= LAMN_SubID))
		{
			if(Starting)
				Starting = FALSE;
			else
			{
					// Check the type

				switch(Template.nm_Type)
				{
						// Found a new menu?

					case NM_TITLE:

						DB(kprintf("NM_TITLE |%s|\n",Template.nm_Label));

							// Stop on weird labels

						if(Template.nm_Label == NM_BARLABEL)
							Error = ERROR_OBJECT_WRONG_TYPE;
						else
						{
								// Choose the menu to use

							if(!LastMenu)
								Menu = (MenuNode *)&Root->Node;
							else
								Menu = NULL;

							DB(kprintf("         make menu\n"));

								// Make room for the menu

							if(Menu = LTP_MakeMenu(Root,Menu,&Template))
							{
								Menu->ID = ID;

								ID = 0;

									// Link to last menu

								if(LastMenu)
									LastMenu->Menu.NextMenu = &Menu->Menu;

									// Mark the last menu item

								if(LastSub)
									LastSub->Flags |= ITEMF_LastItem;
								else
								{
									if(LastItem)
										LastItem->Flags |= ITEMF_LastItem;
								}

								LastMenu	= Menu;
								LastItem	= NULL;
								LastSub		= NULL;

									// Add the menu to the list

								AddTail((struct List *)&Root->MenuList,(struct Node *)Menu);
							}
							else
								Error = ERROR_NO_FREE_STORE;
						}

						break;

						// Found a new menu item?

					case NM_ITEM:

						DB(kprintf("NM_ITEM |%s|\n",Template.nm_Label == NM_BARLABEL ? (STRPTR)"«Bar»" : Template.nm_Label));

							// We need a menu to attach this to

						if(!LastMenu)
							Error = ERROR_INVALID_COMPONENT_NAME;
						else
						{
							DB(kprintf("        make item\n"));

								// Make room for the item

							if(Item = LTP_MakeItem(Root,&Template))
							{
									// Put the char/code/qualifier in

								Item->Char		= Char;
								Item->Code		= Code;
								Item->Qualifier	= Qualifier;
								Item->ID		= ID;

								ID = 0;

									// Link it in

								if(LastItem)
									LastItem->Item.NextItem = &Item->Item;
								else
									LastMenu->Menu.FirstItem = &Item->Item;

								LastItem	= Item;
								LastSub		= NULL;

									// Add it to the item list

								AddTail((struct List *)&Root->ItemList,(struct Node *)Item);
							}
							else
								Error = ERROR_NO_FREE_STORE;
						}

						break;

					case NM_SUB:

						DB(kprintf("NM_SUB |%s|\n",Template.nm_Label == NM_BARLABEL ? (STRPTR)"«Bar»" : Template.nm_Label));

							// We need a menu item to attach this to

						if(!LastItem)
							Error = ERROR_INVALID_COMPONENT_NAME;
						else
						{
							DB(kprintf("       make sub\n"));

								// Make room for the item

							if(Sub = LTP_MakeItem(Root,&Template))
							{
									// Put the char/code/qualifier in

								Sub->Char		= Char;
								Sub->Code		= Code;
								Sub->Qualifier	= Qualifier;
								Sub->ID			= ID;

								ID = 0;

									// Make sure it stays a submenu item

								Sub->Flags |= ITEMF_IsSub;

									// Link it in

								if(LastSub)
									LastSub->Item.NextItem = &Sub->Item;
								else
								{
										// We cannot attach submenu items to
										// separator bars

									if(LastItem->Flags & ITEMF_IsBar)
										Error = ERROR_INVALID_COMPONENT_NAME;
									else
									{
										DB(kprintf("-----------> |%s| has submenu items\n",((struct IntuiText *)LastItem->Item.ItemFill)->IText));

											// Link the submenu item in

										LastItem->Item.SubItem = &Sub->Item;

											// Zap the command sequence data

										LastItem->Flags		= (LastItem->Flags & ~ITEMF_Command) | ITEMF_HasSub;
										LastItem->ExtraLabel	= NULL;

										LastItem->Item.Flags	&= ~COMMSEQ;
										LastItem->Item.Command	 = 0;

											// This is the first submenu item for this menu item

										Sub->Flags |= ITEMF_FirstSub;
									}
								}

								LastSub = Sub;

									// Add it to the list

								AddTail((struct List *)&Root->ItemList,(struct Node *)Sub);
							}
							else
								Error = ERROR_NO_FREE_STORE;
						}

						break;
				}

				memset(&Template,0,sizeof(Template));
			}
		}

		if(Entry)
		{
			switch(Entry->ti_Tag)
			{
				case LAMN_TitleText:

					Template.nm_Type	= NM_TITLE;
					Template.nm_Label	= (STRPTR)Entry->ti_Data;

					break;

				case LAMN_TitleID:

					if(!Root->LocaleHook)
					{
						Error = ERROR_OBJECT_NOT_FOUND;
						break;
					}

					Template.nm_Type	= NM_TITLE;
					Template.nm_Label	= (STRPTR)CallHookPkt(Root->LocaleHook,Root->Handle,(APTR)Entry->ti_Data);
					break;

				case LAMN_ItemText:

					Template.nm_Type	= NM_ITEM;
					Template.nm_Label	= (STRPTR)Entry->ti_Data;

					if(Template.nm_Label != NM_BARLABEL && !Template.nm_Label[1])
					{
						if(Template.nm_Label[0])
							Template.nm_CommKey = Template.nm_Label;

						Template.nm_Label = &Template.nm_Label[2];
					}

					break;

				case LAMN_ItemID:

					if(!Root->LocaleHook)
					{
						Error = ERROR_OBJECT_NOT_FOUND;
						break;
					}

					Template.nm_Type	= NM_ITEM;
					Template.nm_Label	= (STRPTR)CallHookPkt(Root->LocaleHook,Root->Handle,(APTR)Entry->ti_Data);

					if(Template.nm_Label != NM_BARLABEL && !Template.nm_Label[1])
					{
						if(Template.nm_Label[0])
							Template.nm_CommKey = Template.nm_Label;

						Template.nm_Label = &Template.nm_Label[2];
					}

					break;

				case LAMN_SubText:

					Template.nm_Type	= NM_SUB;
					Template.nm_Label	= (STRPTR)Entry->ti_Data;

					if(Template.nm_Label != NM_BARLABEL && !Template.nm_Label[1])
					{
						if(Template.nm_Label[0])
							Template.nm_CommKey = Template.nm_Label;

						Template.nm_Label = &Template.nm_Label[2];
					}

					break;

				case LAMN_SubID:

					if(!Root->LocaleHook)
					{
						Error = ERROR_OBJECT_NOT_FOUND;
						break;
					}

					Template.nm_Type	= NM_SUB;
					Template.nm_Label	= (STRPTR)CallHookPkt(Root->LocaleHook,Root->Handle,(APTR)Entry->ti_Data);

					if(Template.nm_Label != NM_BARLABEL && !Template.nm_Label[1])
					{
						if(Template.nm_Label[0])
							Template.nm_CommKey = Template.nm_Label;

						Template.nm_Label = &Template.nm_Label[2];
					}

					break;

				case LAMN_KeyText:

					Template.nm_CommKey = (STRPTR)Entry->ti_Data;
					break;

				case LAMN_KeyID:

					if(!Root->LocaleHook)
					{
						Error = ERROR_OBJECT_NOT_FOUND;
						break;
					}

					Template.nm_CommKey = (STRPTR)CallHookPkt(Root->LocaleHook,Root->Handle,(APTR)Entry->ti_Data);
					break;

				case LAMN_CommandText:

					Template.nm_CommKey	= (STRPTR)Entry->ti_Data;
					Template.nm_Flags	= Template.nm_Flags | NM_COMMANDSTRING;
					break;

				case LAMN_CommandID:

					if(!Root->LocaleHook)
					{
						Error = ERROR_OBJECT_NOT_FOUND;
						break;
					}

					Template.nm_CommKey	= (STRPTR)CallHookPkt(Root->LocaleHook,Root->Handle,(APTR)Entry->ti_Data);
					Template.nm_Flags	= Template.nm_Flags | NM_COMMANDSTRING;
					break;

				case LAMN_MutualExclude:

					Template.nm_MutualExclude = (ULONG)Entry->ti_Data;
					break;

				case LAMN_UserData:

					Template.nm_UserData = (APTR)Entry->ti_Data;
					break;

				case LAMN_Disabled:

					if(Entry->ti_Data)
					{
						if(Template.nm_Type == NM_TITLE)
							Template.nm_Flags |= NM_MENUDISABLED;
						else
							Template.nm_Flags |= NM_ITEMDISABLED;
					}
					else
					{
						if(Template.nm_Type == NM_TITLE)
							Template.nm_Flags &= ~NM_MENUDISABLED;
						else
							Template.nm_Flags &= ~NM_ITEMDISABLED;
					}

					break;

				case LAMN_CheckIt:

					if(Template.nm_Type != NM_TITLE)
					{
						if(Entry->ti_Data)
							Template.nm_Flags |= CHECKIT;
						else
							Template.nm_Flags &= ~CHECKIT;
					}

					break;

				case LAMN_Checked:

					if(Template.nm_Type != NM_TITLE)
					{
						if(Entry->ti_Data)
							Template.nm_Flags |= CHECKIT | CHECKED;
						else
							Template.nm_Flags &= ~CHECKED;
					}

					break;

				case LAMN_Toggle:

					if(Template.nm_Type != NM_TITLE)
					{
						if(Entry->ti_Data)
							Template.nm_Flags |= CHECKIT | MENUTOGGLE;
						else
							Template.nm_Flags &= ~MENUTOGGLE;
					}

					break;

				case LAMN_Code:

					Code = Entry->ti_Data;
					Char = 0;

					break;

				case LAMN_Qualifier:

					Qualifier = Entry->ti_Data;
					break;

				case LAMN_Char:

					Char = Entry->ti_Data;
					Code = 0;

					break;

				case LAMN_ID:

					ID = (ULONG)Entry->ti_Data;
					break;
			}
		}
		else
		{
				// Mark the last menu item

			if(LastSub)
				LastSub->Flags |= ITEMF_LastItem;
			else
			{
				if(LastItem)
					LastItem->Flags |= ITEMF_LastItem;
			}

			break;
		}
	}

		// Did we create anything at all?

	if(!Error)
	{
		if(!Root->MenuList.mlh_Head->mln_Succ)
			Error = ERROR_OBJECT_NOT_FOUND;
		else
			LTP_FixExtraLabel(Root,&Error);
	}

	if(ErrorPtr)
		*ErrorPtr = Error;

	DB(kprintf("through, error=%ld\n",Error));

	if(Error)
		return(FALSE);
	else
		return(TRUE);
}

#endif	/* DO_MENUS */
