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


struct IntString
{
  LONG value;
  LONG max;
  LONG min;
};

#define TOGGLE_KIND 10
#define GADGDEFAULT 0x8000	// default (return) gadget
#define GADGMAXSIZE 0x4000	// make gadgets as wide as possible
#define GADGTEXTNUC 0x2000	// no underscore converting on TEXT_KIND
#define GADGTEXTLEFT 0x1000	// left justify TEXT_KIND (default=center)
#define GADGTEXTRIGHT 0x0800	// right justify TEXT_KIND (default=center)
#define GADGEOF 0x0001		// end of line

// bits 16-23 = same size number
// bits 24-31 = gadgetid number

#define GADSCREEN 0		// open on struct Screen*
#define GADCENTER 1		// center window
#define GADSUPER 2		// super table

//supertable;
//16-24 = clicked gadget,8-15=gadget's value,0-7=number of modify lines
//SENABLE/SDISABLE/SCHANGE,0-7 gadget to be modified

#define SDISABLE 0x80000000
#define SCHANGE 0x40000000
#define SENABLE 0

WORD reqwindow (ULONG *);
void reqwindowtext (UBYTE *);
void dobutton (WORD *, ULONG *, WORD, ULONG);
void donumbutton (WORD *, ULONG *, WORD, WORD, ULONG);
void donumbuttonstring (WORD *, ULONG *, UBYTE *, WORD, ULONG);
void doletterbutton (WORD *, ULONG *, WORD, WORD, ULONG);
void doletterbuttonstring (WORD *, ULONG *, UBYTE *, WORD, ULONG);
void dotext (WORD *, ULONG *, WORD, ULONG);
void dostring (WORD *, ULONG *, UBYTE *, ULONG);
void dobuttonstring (WORD *, ULONG *, UBYTE *, ULONG);
ULONG *doswitch (WORD *, ULONG *, WORD, WORD, ULONG);
ULONG *doswitchstring (WORD *, ULONG *, UBYTE *, WORD, ULONG);
ULONG *dotoggle (WORD *, ULONG *, WORD, WORD, ULONG);
ULONG *dotogglestring (WORD *, ULONG *, UBYTE *, WORD, ULONG);
ULONG *dointegergad (WORD *, ULONG *, struct IntString *, ULONG);
void dostringgad (WORD *, ULONG *, UBYTE *, WORD, ULONG);
ULONG *docyclenumber (WORD *, ULONG *, ULONG, WORD, WORD, WORD);
ULONG *docyclestring (WORD *, ULONG *, WORD, ULONG, WORD, UBYTE *, ...);
ULONG *docycle (WORD *, ULONG *, WORD, ULONG, WORD, WORD, ...);
WORD pressgadget (struct Window *, struct Gadget *);
void unpressgadget (struct Window *, struct Gadget *);

ULONG bguirequest (UBYTE *, UBYTE *, WORD, UBYTE **, void *, ...);
