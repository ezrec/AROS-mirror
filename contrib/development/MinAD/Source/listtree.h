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

#ifndef LISTTREE_H
#define LISTTREE_H 1

/*------------------------------------------------------------------------*/

extern struct AutoDocs autodocs;

/*------------------------------------------------------------------------*/

void InitListtree(void);
void ExitListtree(void);
void ShowListtree(void);
void UpdateListtree(void);
void DirRide(void);
void ShowFunc(void);
void Search(void);
void SelectFromSearch(void);
void Filter(void);
void FilterClear(void);
void OpenCloseAll(BOOL mode);

void mTools_MakeGuide(void);

/*------------------------------------------------------------------------*/

#endif /* LISTTREE_H */

