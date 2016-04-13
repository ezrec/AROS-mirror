/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#ifndef TAGS_H
#define TAGS_H 1

/*------------------------------------------------------------------------*/

#define MSERIAL_NATMEG 									2099480043
#define TAGBASE_NATMEG									(TAG_USER | (MSERIAL_NATMEG << 16))

/* Function tags */
#define MUIV_ShowFunc									(0x0001)
#define MUIV_DirRide										(0x0002)
#define MUIV_UpdateListtree							(0x0003)
#define MUIV_UpdateListtreeMode						(0x0004)
#define MUIV_UpdateListtreeHitEnter					(0x0005)
#define MUIV_Search										(0x0006)
#define MUIV_SelectFromSearch							(0x0007)
#define MUIV_Filter										(0x0008)
#define MUIV_FilterClear								(0x0009)
#define MUIV_OpenAll										(0x000A)
#define MUIV_CloseAll									(0x000B)

/* Menu tags */
#define MUIV_mProjct_About								(0xF001)
#define MUIV_mProjct_AboutMUI							(0xF002)
#define MUIV_mProjct_Iconify							(0xF003)
#define MUIV_mTools_Search								(0xF011)
#define MUIV_mTools_MakeGuide							(0xF012)

/*------------------------------------------------------------------------*/

#endif /* TAGS_H */

