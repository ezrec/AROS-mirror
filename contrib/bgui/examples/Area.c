/*
 * @(#) $Header$
 *
 * Area.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1994-1995 Jan van den Baard.
 * (C) Copyright 1994-1995 Jaba Development.
 * All Rights Reserved.
 *
 * Description: Very simple test program for the area class.
 *
 * $Log$
 * Revision 42.1  2000/08/10 11:50:54  chodorowski
 * Cleaned up and prettyfied the GUIs a bit.
 *
 * Revision 42.0  2000/05/09 22:19:19  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:10  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:31  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:44:49  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE v3.0
dcc Area.c -proto -mi -ms -lbgui
quit
*/

#include "DemoCode.h"
#include <intuition/icclass.h>

#define ID_REDRAW_AREA          1       /* When this ID is encountered we (re-)render the area. */
#define ID_QUIT            2

/*
**      Simply took from the old BGUIDemo. Renders a simple
**      integer mandel in the area.
**/

ULONG RenderMandel( struct Window *win, struct IBox *ibox, Object *area, Object *wd )
{
	LONG    zr, zi, ar, ai, dr, di, sr, si, st, x, y, i;
	LONG    xsize, ysize, depth;
	ULONG rc;

	depth = GetBitMapAttr(win->WScreen->RastPort.BitMap, BMA_DEPTH);
	xsize = ibox->Width;
	ysize = ibox->Height;

	sr = 0x400000 / xsize;
	si = 0x300000 / ysize;
	st = 0x140000 * -2;
	zi = 0x180000;

	for (y = ysize - 1; y >= 0; y--)
	{
		zi -= si;
		zr = st;
		for (x = 0; x < xsize; x++)
		{
			i = 0;
			ar = zr;
			ai = zi;
			do {
				dr = ar >> 10;
				di = ai >> 10;
				ai = dr * 2 * di + zi;
				dr *= dr;
				di *= di;
				ar = dr - di + zr;
				i++;
			} while (( i <= 25 ) && (( dr + di ) <= 0x400000));
			SetAPen(win->RPort, i % (1 << depth));
			WritePixel(win->RPort, x + ibox->Left, y + ibox->Top);
			/*
			**      To keep things simple I sortof duplicated
			**      the event handler here. It simply returns
			**      to the main event handler (below) when
			**      necessary.
			**/
			while ((rc = HandleEvent( wd )) != WMHI_NOMORE)
			{
				if (rc == ID_REDRAW_AREA || rc == ID_QUIT || rc == WMHI_CLOSEWINDOW)
					return rc;
			}

			zr += sr;
		}
	}
	return( 0L );
}

/*
**      Here we go...
**/
void StartDemo(void)
{
	struct Window   *w;
	Object          *Win, *Area, *But;
	IPTR             signal;
	ULONG            rc;
	struct IBox     *area_box;
	BOOL             running = TRUE;

	/*
	**      Make a window.
	**/
	Win = WindowObject,
		WINDOW_Title,                           "AreaDemo",
		WINDOW_AutoAspect,              TRUE,
		WINDOW_SmartRefresh,            TRUE,
		WINDOW_AutoKeyLabel,            TRUE,
		WINDOW_ScaleHeight,               15,
		WINDOW_ScaleWidth,                15,
		WINDOW_MasterGroup,
			VGroupObject, HOffset(4), VOffset(4), Spacing(4),
				GROUP_BackFill,         SHINE_RASTER,
				StartMember,
					/*
					**      Create AreaClass object.
					**
					**      Note the usage of the ICA_TARGET attribute. This is
					**      required otherwise the object will never notify you
					**      of size changes!!
					**/
					Area = AreaObject,
						FRM_Type,                       FRTYPE_BUTTON,
						FRM_EdgesOnly,          TRUE,
					   AREA_MinWidth,               40,
						AREA_MinHeight, 10,
						GA_ID,                          ID_REDRAW_AREA,
						ICA_TARGET,                     ICTARGET_IDCMP,
					EndObject,
				EndMember,
				StartMember,
					But = PrefButton("_Quit", ID_QUIT), FixMinHeight,
				EndMember,
			EndObject,
		EndObject;

	/*
	**      OK?
	**/
	if (Win)
	{
		/*
		**      Open the window.
		**/
		if ((w = WindowOpen(Win)))
		{
			/*
			**      Get window signal.
			**/
			GetAttr(WINDOW_SigMask, Win, &signal);
			/*
			**      Poll messages...
			**/
			do
			{
				Wait(signal);
				while ((rc = HandleEvent(Win)) != WMHI_NOMORE)
				{

					handleMsg:
					switch ( rc )
					{
					case ID_REDRAW_AREA:
						/*
						**      Were signalled to redraw the
						**      area. Obtain the area bounds.
						**/
						GetAttr(AREA_AreaBox, Area, (IPTR *)&area_box);
						/*
						**      Render inside the area.
						**      When this routine returns we
						**      evaluate the return code.
						**/
						if ((rc = RenderMandel(w, area_box, Area, Win)))
							goto handleMsg;

						break;

					case    WMHI_CLOSEWINDOW:
					case    ID_QUIT:
						running = FALSE;
						break;
					}
				}
			} while (running);
		}
		DisposeObject(Win);
	}
}
