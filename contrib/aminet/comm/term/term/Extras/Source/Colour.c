/*
**	Colour.c
**
**	Colour table support code
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* ColourTablex12(ColourTable *Table,UWORD *Dest12,LONG NumColours):
	 *
	 *	Convert colour table into 12 bit precision colours.
	 */

STATIC VOID
ColourTablex12(ColourTable *Table,UWORD *Dest12,LONG NumColours)
{
	LONG i;

	for(i = 0 ; i < NumColours ; i++)
		Colour96x12(&Table->Entry[i].Red,&Dest12[i],1);
}

	/* Colour96xColourTable(ULONG *Source96,ColourTable *Table,LONG NumColours):
	 *
	 *	Convert 96 bit precision colours for colour table.
	 */

VOID
Colour96xColourTable(ULONG *Source96,ColourTable *Table,LONG NumColours)
{
	LONG i;

	for(i = 0 ; i < NumColours ; i++)
	{
		Table->Entry[i].Red		= *Source96++;
		Table->Entry[i].Green	= *Source96++;
		Table->Entry[i].Blue	= *Source96++;
	}
}

	/* Colour12xColourTable(UWORD *Source12,ColourTable *Table,LONG NumColours):
	 *
	 *	Convert 12 bit precision colours for colour table.
	 */

VOID
Colour12xColourTable(UWORD *Source12,ColourTable *Table,LONG NumColours)
{
	LONG i;

	for(i = 0 ; i < NumColours ; i++)
		Colour12x96(&Source12[i],&Table->Entry[i].Red,1);
}

	/* Colour12x96(UWORD *Source12,ULONG *Dest96,LONG NumColours):
	 *
	 *	Convert 12 bit precision colours into 96 bit colours.
	 */

VOID
Colour12x96(UWORD *Source12,ULONG *Dest96,LONG NumColours)
{
	ULONG r,g,b;
	LONG v;

	do
	{
		v = *Source12++;

		r = (v & 0xF00) >> 8;
		g = (v & 0x0F0) >> 4;
		b = (v & 0x00F);

		*Dest96++ = SPREAD((r << 4) | r);
		*Dest96++ = SPREAD((g << 4) | g);
		*Dest96++ = SPREAD((b << 4) | b);
	}
	while(--NumColours);
}

	/* Colour96x12(UWORD *Source12,ULONG *Dest96,LONG NumColours):
	 *
	 *	Convert 96 bit precision colours into 12 bit colours.
	 */

VOID
Colour96x12(ULONG *Source96,UWORD *Dest12,LONG NumColours)
{
	ULONG r,g,b;

	do
	{
		r = (*Source96++) >> 28;
		g = (*Source96++) >> 28;
		b = (*Source96++) >> 28;

		*Dest12++ = (r << 8) | (g << 4) | b;
	}
	while(--NumColours);
}

	/* CopyColourEntry(ColourTable *Source,ColourTable *Destination,LONG From,LONG To):
	 *
	 *	Copy one colour register from one place to another.
	 */

VOID
CopyColourEntry(ColourTable *Source,ColourTable *Destination,LONG From,LONG To)
{
	if(From >= 0 && To >= 0 && From < Source->NumColours && To < Destination->NumColours)
	{
		Destination->Entry[To].Red		= Source->Entry[From].Red;
		Destination->Entry[To].Green	= Source->Entry[From].Green;
		Destination->Entry[To].Blue		= Source->Entry[From].Blue;
	}
}

	/* ColourTablex96(ColourTable *Table,ULONG *Dest96):
	 *
	 *	Convert colour table to 96 bit RGB palette.
	 */

VOID
ColourTablex96(ColourTable *Table,ULONG *Dest96)
{
	LONG i;

	for(i = 0 ; i < Table->NumColours ; i++)
	{
		*Dest96++ = Table->Entry[i].Red;
		*Dest96++ = Table->Entry[i].Green;
		*Dest96++ = Table->Entry[i].Blue;
	}
}

	/* DeleteColourTable(ColourTable *Table):
	 *
	 *	Delete a colour table.
	 */

VOID
DeleteColourTable(ColourTable *Table)
{
	FreeVecPooled(Table);
}

	/* CreateColourTable(LONG NumEntries,UWORD *Source12,ULONG *Source96):
	 *
	 *	Create a colour table and fill it with given data.
	 */

ColourTable *
CreateColourTable(LONG NumEntries,UWORD *Source12,ULONG *Source96)
{
	ColourTable *Record;

	if(Record = (ColourTable *)AllocVecPooled(sizeof(ColourTable) + NumEntries * sizeof(ColourEntry),MEMF_ANY | MEMF_CLEAR))
	{
		LONG i;

		Record->NumColours = NumEntries;

		for(i = 0 ; i < NumEntries ; i++)
		{
			Record->Entry[i].One	= 1;
			Record->Entry[i].Which	= i;
		}

		Record->Entry[NumEntries].One = 0;

		if(Source12)
			Colour12xColourTable(Source12,Record,NumEntries);

		if(Source96)
			Colour96xColourTable(Source96,Record,NumEntries);
	}

	return(Record);
}

	/* LoadColours(struct ViewPort *VPort,ColourTable *Table,UWORD *Colour12,LONG NumColours):
	 *
	 *	Load a colour table.
	 */

VOID
LoadColourTable(struct ViewPort *VPort,ColourTable *Table,UWORD *Colour12,LONG NumColours)
{
	if(Kick30 && Table)
		LoadRGB32(VPort,(ULONG *)Table->Entry);
	else
	{
		if(Colour12)
			LoadRGB4(VPort,(UWORD *)Colour12,NumColours);
		else
		{
			if(Table)
			{
				UWORD *Dest12;

				if(Dest12 = (UWORD *)AllocVecPooled(sizeof(UWORD) * NumColours,MEMF_ANY))
				{
					ColourTablex12(Table,Dest12,NumColours);

					LoadRGB4(VPort,Dest12,NumColours);

					FreeVecPooled(Dest12);
				}
				else
				{
					LONG i;

					for(i = 0 ; i < Table->NumColours ; i++)
						SetRGB4(VPort,i,Table->Entry[i].Red >> 28,Table->Entry[i].Green >> 28,Table->Entry[i].Blue >> 28);
				}
			}
		}
	}
}

	/* GrabColours(struct ViewPort *VPort,ColourTable *Table):
	 *
	 *	Copies the colour palette of the given viewport into the table.
	 */

VOID
GrabColours(struct ViewPort *VPort,ColourTable *Table)
{
	if(Kick30)
	{
		LONG i;

		for(i = 0 ; i < Table->NumColours ; i++)
			GetRGB32(VPort->ColorMap,i,1,(ULONG *)&Table->Entry[i].Red);
	}
	else
	{
		LONG	Value;
		LONG	i;

		for(i = 0 ; i < Table->NumColours ; i++)
		{
			Value = GetRGB4(VPort->ColorMap,i);

			Table->Entry[i].Red		= ((Value & 0xF00) >> 8) * 0x11111111;
			Table->Entry[i].Green	= ((Value & 0x0F0) >> 4) * 0x11111111;
			Table->Entry[i].Blue	=  (Value & 0x00F)       * 0x11111111;
		}
	}
}

	/* CopyColours(ColourTable *From,ColourTable *To):
	 *
	 *	Copy colours from one table to another.
	 */

VOID
CopyColours(ColourTable *From,ColourTable *To)
{
	LONG i,Min;

	Min = MIN(From->NumColours,To->NumColours);

	for(i = 0 ; i < Min ; i++)
	{
		To->Entry[i].Red	= From->Entry[i].Red;
		To->Entry[i].Green	= From->Entry[i].Green;
		To->Entry[i].Blue	= From->Entry[i].Blue;
	}
}
