/*
     Filemaster - Multitasking directory utility.
     Copyright (C) 2000  Toni Wilen
     
     This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
     as published by the Free Software Foundation; either version 2
     of the License, or (at your option) any later version.
     
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//void initproc(struct FMList*,UBYTE*);
void endproc (struct FMList *);
WORD setalloc (struct FMList *, WORD);
struct ProcMsg *sinitproc (void);
void deinitproc (struct ProcMsg *);
WORD tstsubproc (WORD);
WORD testabort (struct FMList *);
WORD askabort (struct FMList *);
void changename (struct FMList *, UBYTE *);
//WORD launch(APTR,struct CMenuConfig*,WORD,WORD);
//void priority(struct CMenuConfig*);

struct ProcMsg
{
  struct CMenuConfig *cmc;
};

struct FMPort
{
  struct MsgPort msgport;
  struct FMMain *fmmain;
  struct FMConfig *fmconfig;
};
