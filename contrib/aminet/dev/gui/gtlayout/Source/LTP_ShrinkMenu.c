/*
**  GadTools layout toolkit
**
**  Copyright © 1993-1998 by Olaf `Olsen' Barthel
**      Freely distributable.
**
**  :ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

#include "Assert.h"

#ifdef DO_MENUS	/* Support code */

    /* LTP_ShrinkMenu(RootMenu *Root,ItemNode *First,ItemNode *Last,UWORD Mask):
     *
     *  Rethink the widths of all menu items between First and Last, including
     *  both the first and the last entry.
     */

VOID
LTP_ShrinkMenu(RootMenu *Root,ItemNode *First,ItemNode *Last,LONG Mask)
{
    ItemNode    *Here;
    LONG         Width,MaxWidth = 0,CommandWidth = 0;

        // Determine the widths of all items, also calculate
        // the command widths

    for(Here = First ; Here->Node.mln_Succ ; Here = (ItemNode *)Here->Node.mln_Succ)
    {
        if((Here->Flags & ITEMF_IsSub) == Mask)
        {
            if(!(Here->Flags & ITEMF_IsBar))
            {
                struct IntuiText *IntuiText = (struct IntuiText *)Here->Item.ItemFill;

                Width = 2 + TextLength(&Root->RPort,IntuiText->IText,strlen(IntuiText->IText)) + 2;

                if(Here->Item.Flags & CHECKIT)
                    Width += 2 + Root->CheckWidth;

                if(Width > MaxWidth)
                    MaxWidth = Width;

                if((Width = LTP_GetCommandWidth(Root,Here)) > CommandWidth)
                    CommandWidth = Width;
            }
        }

        if(Here == Last)
            break;
    }

        // Now adjust the widths of all objects

    for(Here = First ; Here->Node.mln_Succ ; Here = (ItemNode *)Here->Node.mln_Succ)
    {
        if((Here->Flags & ITEMF_IsSub) == Mask)
        {
            if(Here->Flags & ITEMF_IsBar)
            {
                struct Image *Image = (struct Image *)Here->Item.ItemFill;

                Image->Width = MaxWidth + CommandWidth - 4;
            }
            else
            {
                    // Move over the submenu items if necessary

                if(Here->Item.SubItem)
                {
                    ItemNode *Sub = (ItemNode *)((IPTR)Here->Item.SubItem - sizeof(struct MinNode));

                    FOREVER
                    {
                        Sub->Item.LeftEdge = MaxWidth + 2;

                        if(Sub->Item.NextItem)
                            Sub = (ItemNode *)Sub->Node.mln_Succ;
                        else
                            break;
                    }
                }
            }

            Here->Item.Width = MaxWidth + CommandWidth;
        }

        if(Here == Last)
            break;
    }
}

#endif  /* DO_MENUS */
