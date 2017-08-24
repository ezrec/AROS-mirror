/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 * @author Pavel Fedin <sonic_amiga@mail.ru>
 */

#ifndef SCOUT_DEFS_H
#define SCOUT_DEFS_H

/*************************************************************/
/*                      User definitions                     */
/*************************************************************/

#define FILENAME_LENGTH     512
#define VERSION_LENGTH      16
#if (__WORDSIZE == 64)
#define ADDRESS_LENGTH      32
#define ADDRESS_FORMAT      "$%016lx"
#else
#define ADDRESS_LENGTH      16
#define ADDRESS_FORMAT      "$%08lx"
#endif
#define PATH_LENGTH         512
#define PATH_LENGTH_CHAR    "512"
#define NODENAME_LENGTH     512
#define TEXT_LENGTH         512
#define TEXT_LENGTH_CHAR    "512"
#define NODETYPE_LENGTH     16
#define NUMBER_LENGTH       16
#define TASK_STATE_LENGTH   16
#define TMP_STRING_LENGTH   1024
#define TS_FROZEN           0xff
#define PATTERN_LENGTH      (PATH_LENGTH * 2 + 2)
#define NODESUBSYS_LENGTH   32
#define NODESUBTYPE_LENGTH  32

static inline struct List *ListPtr(APTR list)
{
    return (struct List *)list;
}

#define ITERATE_LIST(list, type, node)                  for (node = (type)ListPtr(list)->lh_Head; ((struct Node *)node)->ln_Succ; node = (type)((struct Node *)node)->ln_Succ)
#define ITERATE_LIST_REVERSE(list, type, node)          for (node = (type)ListPtr(list)->lh_TailPred; ((struct Node *)node)->ln_Pred; node = (type)((struct Node *)node)->ln_Pred)

#define MakeID(a,b,c,d)     ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

#define MyTextFrame         TextFrame,MUIA_Background,MUII_TextBack

#define LabelA(label)   TextObject,MUIA_Text_PreParse,(IPTR)"\033r",\
            MUIA_Text_Contents,(IPTR)label,TextFrame,MUIA_FramePhantomHoriz,\
            TRUE,End

#define LabelB(label)   TextObject,MUIA_Text_PreParse,(IPTR)"\033r",\
            MUIA_Text_Contents,(IPTR)label,TextFrame,MUIA_FramePhantomHoriz,\
            TRUE,MUIA_Weight,0,MUIA_InnerLeft,0,MUIA_InnerRight,0,End

#define LabelC(label)   TextObject,MUIA_Text_PreParse,(IPTR)"\033c",\
            MUIA_Text_Contents,(IPTR)label,TextFrame,MUIA_FramePhantomHoriz,\
            TRUE,End

#ifndef ZERO
    #define ZERO            ((BPTR)0L)
#endif

#endif /* SCOUT_DEFS_H */
