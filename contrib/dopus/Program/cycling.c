/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "dopus.h"

static struct ViewPort *cy_VPort;
static struct Interrupt cy_VBlank;
static CRange *cy_Range;
static LONG cy_RangeNum,cy_ColNum;
static UWORD cy_ColMap[64],cy_TempCol,cy_UndoBuffer[64];
static short cy_TicksPassed[16],cy_WeAreWaiting,cy_Command;
static BOOL cy_CodeRun=FALSE;

void /* __saveds */ colourcycle()
{
	short i,j;

	if (cy_Command==CY_WAIT) {
		if (!cy_WeAreWaiting) LoadRGB4(cy_VPort,cy_UndoBuffer,cy_ColNum);
		cy_WeAreWaiting=TRUE;
		return;
	}
	if (cy_WeAreWaiting) {
		for (i=0;i<cy_ColNum;i++) cy_ColMap[i]=cy_UndoBuffer[i];
		for (i=0;i<cy_RangeNum;i++) cy_TicksPassed[i]=0;
		cy_WeAreWaiting=FALSE;
	}

	for (i=0;i<cy_RangeNum;i++) {
		cy_TicksPassed[i]++;
		if (cy_TicksPassed[i]==cy_Range[i].rate) {
			cy_TicksPassed[i]=0;
			if (!(cy_Range[i].active&CRNG_ACTIVE)) continue;
			if (cy_Range[i].active&CRNG_REVERSE) {
				cy_TempCol=cy_ColMap[cy_Range[i].low];
				for (j=cy_Range[i].low;j<cy_Range[i].high;j++)
					cy_ColMap[j]=cy_ColMap[j+1];
				cy_ColMap[cy_Range[i].high]=cy_TempCol;
			}
			else {
				cy_TempCol=cy_ColMap[cy_Range[i].high];
				for (j=cy_Range[i].high;j>cy_Range[i].low;j--)
					cy_ColMap[j]=cy_ColMap[j-1];
				cy_ColMap[cy_Range[i].low]=cy_TempCol;
			}
			LoadRGB4(cy_VPort,cy_ColMap,cy_ColNum);
		}
	}
}

int initcycle(ViewPort,ColMap,ColNum,Range,RangeNum)
struct ViewPort *ViewPort;
UWORD *ColMap;
int ColNum;
CRange *Range;
int RangeNum;
{
	register short i;

	if (!ViewPort || !ColMap || !ColNum || ColNum>64 || !Range || !RangeNum) return(FALSE);

	cy_VPort=ViewPort;
	cy_ColNum=ColNum;
	cy_Range=Range;
	cy_RangeNum=RangeNum;

	for (i=0;i<ColNum;i++) cy_ColMap[i]=ColMap[i];
	for (i=0;i<cy_RangeNum;i++) cy_TicksPassed[i]=0;
	for (i=0;i<cy_ColNum;i++) cy_UndoBuffer[i]=cy_ColMap[i];

	cy_Command=CY_WAIT;
	cy_WeAreWaiting=TRUE;
	cy_VBlank.is_Data=(APTR)NULL;
	cy_VBlank.is_Code=colourcycle;
	cy_VBlank.is_Node.ln_Succ=NULL;
	cy_VBlank.is_Node.ln_Pred=NULL;
	cy_VBlank.is_Node.ln_Type=NT_INTERRUPT;
	cy_VBlank.is_Node.ln_Pri=0;
	cy_VBlank.is_Node.ln_Name="dopus_cycle";
	AddIntServer(INTB_VERTB,&cy_VBlank);
	cy_CodeRun=TRUE;
	return(TRUE);
}

void stopcycle()
{
	register short i;

	if (!cy_CodeRun) return;
	cy_WeAreWaiting=TRUE;
	cy_Command=CY_WAIT;
	RemIntServer(INTB_VERTB,&cy_VBlank);
	for (i=0;i<cy_ColNum;i++) cy_ColMap[i]=cy_UndoBuffer[i];
	WaitTOF();
	WaitTOF();
	WaitTOF();
	cy_CodeRun=FALSE;
}

void togglecycle()
{
	static short Mode=FALSE;

	Mode^=TRUE;
	cy_Command=(Mode)?CY_CYCL:CY_WAIT;
	WaitTOF();
}

int checkcycling()
{
	return(cy_Command==CY_CYCL);
}
