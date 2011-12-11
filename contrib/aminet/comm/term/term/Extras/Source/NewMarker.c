/*
**	NewMarker.c
**
**	Text block marker routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

/****************************************************************************/

/*#define DB(x)	x*/
#define DB(x)	;

/****************************************************************************/

STATIC VOID
UpdateLineData(MarkerContext *Context)
{
	Context->FirstFullLine	= Context->FirstPartialLine + 1;
	Context->LastFullLine	= Context->LastPartialLine - 1;

	if(Context->FirstFullLine > Context->LastFullLine)
		Context->FirstFullLine = Context->LastFullLine = -1;

	if(Context->FirstPartialLeft == 0 && Context->FirstPartialRight == Context->NumColumns - 1)
	{
		Context->FirstFullLine = Context->FirstPartialLine;

		if(Context->LastFullLine == -1)
			Context->LastFullLine = Context->FirstFullLine;
	}

	if(Context->LastPartialLeft == 0 && Context->LastPartialRight == Context->NumColumns - 1)
		Context->LastFullLine = Context->LastPartialLine;

	DB(kprintf("\033[3HFirstP: %4ld <%4ld,%4ld>\033[K\n",Context->FirstPartialLine,Context->FirstPartialLeft,Context->FirstPartialRight));
	DB(kprintf("FirstF: %4ld\033[K\n",Context->FirstFullLine));
	DB(kprintf(" LastP: %4ld <%4ld,%4ld>\033[K\n",Context->LastPartialLine,Context->LastPartialLeft,Context->LastPartialRight));
	DB(kprintf(" LastF: %4ld\033[K\n",Context->LastFullLine));
	DB(kprintf("   Dir: %4ld\033[K\n",Context->Direction));
}

STATIC VOID
UnhighlightAll(MarkerContext *Context)
{
		/* Clear the selection in the first line. */

	if(Context->FirstPartialLine != Context->FirstFullLine)
		Context->Unhighlight(Context,Context->FirstPartialLine,Context->FirstPartialLeft,Context->FirstPartialRight);

		/* Clear the text in between the first and the last line. */

	if(Context->FirstFullLine != -1 && Context->LastFullLine != -1)
	{
		LONG i;

		for(i = Context->FirstFullLine ; i <= Context->LastFullLine ; i++)
			Context->Unhighlight(Context,i,0,Context->NumColumns - 1);
	}

		/* Clear the selection in the last line. */

	if(Context->LastPartialLine != Context->LastFullLine && Context->LastPartialLine != Context->FirstPartialLine)
		Context->Unhighlight(Context,Context->LastPartialLine,Context->LastPartialLeft,Context->LastPartialRight);
}

STATIC VOID
HighlightAll(MarkerContext *Context)
{
		/* Highlight the first line. */

	if(Context->FirstPartialLine != Context->FirstFullLine)
		Context->Highlight(Context,Context->FirstPartialLine,Context->FirstPartialLeft,Context->FirstPartialRight);

		/* Highlight the text in between the first and the last line. */

	if(Context->FirstFullLine != -1 && Context->LastFullLine != -1)
	{
		LONG i;

		for(i = Context->FirstFullLine ; i <= Context->LastFullLine ; i++)
			Context->Highlight(Context,i,0,Context->NumColumns - 1);
	}

		/* Highlight the last line. */

	if(Context->LastPartialLine != Context->LastFullLine && Context->LastPartialLine != Context->FirstPartialLine)
		Context->Highlight(Context,Context->LastPartialLine,Context->LastPartialLeft,Context->LastPartialRight);
}

struct MarkerContext *
CreateMarkerContext(
	MARKER_AskPosition			AskPosition,
	MARKER_Scroll				Scroll,
	MARKER_Highlight			Highlight,
	MARKER_Highlight			Unhighlight,
	MARKER_TransferStartStop	TransferStartStop,
	MARKER_Transfer				Transfer,
	MARKER_Put					Put,
	MARKER_Put					PutLine,
	MARKER_PickWord				PickWord)
{
	MarkerContext *Context;

	if(Context = (MarkerContext *)AllocVecPooled(sizeof(MarkerContext),MEMF_ANY|MEMF_CLEAR))
	{
		Context->AskPosition		= AskPosition;
		Context->Scroll				= Scroll;
		Context->Highlight			= Highlight;
		Context->Unhighlight		= Unhighlight;
		Context->TransferStartStop	= TransferStartStop;
		Context->Transfer			= Transfer;
		Context->Put				= Put;
		Context->PutLine			= PutLine;
		Context->PickWord			= PickWord;

		ResetMarkerContext(Context);
	}

	return(Context);
}

VOID
ResetMarkerContext(struct MarkerContext *Context)
{
	Context->FirstVisibleLine	= -1;
	Context->NumVisibleLines	= -1;
	Context->NumLines			= -1;

	Context->FirstVisibleColumn	= -1;
	Context->NumVisibleColumns	= -1;
	Context->NumColumns			= -1;

	Context->AnchorColumn		= -1;
	Context->AnchorLine			= -1;

	Context->CurrentColumn		= -1;
	Context->CurrentLine		= -1;

	Context->FirstFullLine		= -1;
	Context->LastFullLine		= -1;

	Context->FirstPartialLine	= -1;
	Context->FirstPartialLeft	= -1;
	Context->FirstPartialRight	= -1;

	Context->LastPartialLine	= -1;
	Context->LastPartialLeft	= -1;
	Context->LastPartialRight	= -1;

}

VOID
UpMarker(struct MarkerContext *Context)
{
	if(Context->CurrentLine != -1)
	{
		UnhighlightAll(Context);

		ResetMarkerContext(Context);
	}
}

VOID
DownMarker(struct MarkerContext *Context,
	LONG FirstVisibleLine,
	LONG NumVisibleLines,
	LONG NumLines,

	LONG FirstVisibleColumn,
	LONG NumVisibleColumns,
	LONG NumColumns)
{
	LONG Left,Top;

	Context->FirstVisibleLine	= FirstVisibleLine;
	Context->NumVisibleLines	= NumVisibleLines;
	Context->NumLines			= NumLines;

	Context->FirstVisibleColumn	= FirstVisibleColumn;
	Context->NumVisibleColumns	= NumVisibleColumns;
	Context->NumColumns			= NumColumns;

	Context->AskPosition(Context,&Left,&Top,MARKERASK_Clipped);

	Context->AnchorColumn		= Left	+ Context->FirstVisibleColumn;
	Context->AnchorLine			= Top	+ Context->FirstVisibleLine;

	Context->CurrentColumn		= Context->AnchorColumn;
	Context->CurrentLine		= Context->AnchorLine;

	Context->FirstPartialLine	= Context->CurrentLine;
	Context->FirstPartialLeft	= Context->CurrentColumn;
	Context->FirstPartialRight	= Context->CurrentColumn;

	Context->LastPartialLine	= Context->FirstPartialLine;
	Context->LastPartialLeft	= Context->FirstPartialLeft;
	Context->LastPartialRight	= Context->FirstPartialRight;

	UpdateLineData(Context);
	HighlightAll(Context);
}

VOID
MoveMouseMarker(struct MarkerContext *Context)
{
	LONG ScrollLeft,ScrollTop;
	LONG Left,Top,i;

		/* Get the current mouse position. The dimensions are in
		 * local coordinates and need to be translated into
		 * global coordinates. Note that the coordinates may be
		 * larger or smaller than the currently visible portion
		 * of the display.
		 */

	Context->AskPosition(Context,&Left,&Top,MARKERASK_Scroll);

	ScrollLeft = ScrollTop = 0;

		/* If the horizontal position is negative, scroll left one column. */

	if(Left < 0)
	{
			/* Check if there is another column to scroll into view. */

		if(Context->FirstVisibleColumn > 0)
		{
			ScrollLeft = -1;

			Context->FirstVisibleColumn--;
		}

			/* Normalize the position. */

		Left = 0;
	}
	else
	{
			/* If the horizontal position is larger than the visible region,
			 * scroll right one column.
			 */

		if(Left >= Context->NumVisibleColumns)
		{
				/* Check if there is another column to scroll into view. */

			if(Context->FirstVisibleColumn + Context->NumVisibleColumns < Context->NumColumns)
			{
				ScrollLeft = 1;

				Context->FirstVisibleColumn++;
			}

				/* Normalize the position. */

			Left = Context->NumVisibleColumns - 1;
		}
	}

		/* If the vertical position is negative, scroll up one line. */

	if(Top < 0)
	{
			/* Check if there is another line to scroll into view. */

		if(Context->FirstVisibleLine > 0)
		{
			ScrollTop = -1;

			Context->FirstVisibleLine--;
		}

			/* Normalize the position. */

		Top = 0;
	}
	else
	{
			/* If the vertical position is larger than the visible region,
			 * scroll down one line.
			 */

		if(Top >= Context->NumVisibleLines)
		{
				/* Check if there is another line to scroll into view. */

			if(Context->FirstVisibleLine + Context->NumVisibleLines < Context->NumLines)
			{
				ScrollTop = 1;

				Context->FirstVisibleLine++;
			}

				/* Normalize the position. */

			Top = Context->NumVisibleLines - 1;
		}
	}

		/* Scroll the display if we have to. */

	if(ScrollLeft != 0 || ScrollTop != 0)
		Context->Scroll(Context,ScrollLeft,ScrollTop);

		/* Translate into global coordinates. */

	Left	+= Context->FirstVisibleColumn;
	Top		+= Context->FirstVisibleLine;

		/* Check if the mouse did move at all. */

	if(Left != Context->CurrentColumn || Top != Context->CurrentLine)
	{
		LONG NewDir;

		if(Top < Context->AnchorLine)
			NewDir = -1;
		else if (Top > Context->AnchorLine)
			NewDir =  1;
		else
		{
			if(Left < Context->AnchorColumn)
				NewDir = -1;
			else
				NewDir =  1;
		}

		if(Context->Direction == 0)
			Context->Direction = NewDir;

		DB(kprintf("\033[9HTop <%4ld> Last <%4ld,%4ld> Anchor <%4ld,%4ld> Dir <%2ld> NewDir <%2ld>\033[K\n",Top,Context->CurrentLine,Context->CurrentColumn,Context->AnchorLine,Context->AnchorColumn,Context->Direction,NewDir));

			/* Check for "crossover", i.e. if the mouse moved to the other
			 * side of the marker anchor.
			 */

		if(Context->Direction != NewDir)
		{
				/* Unhighlight the highlighted lines. */

			UnhighlightAll(Context);

				/* Now check where the mouse moved to in relation to the anchor. */

			if(Top < Context->AnchorLine)
			{
				DB(kprintf("\033[8H(1)\033[K\n"));

					/* Mouse is above the marker anchor. */

				Context->Direction = -1;

				Context->FirstPartialLine	= Top;
				Context->FirstPartialLeft	= Left;
				Context->FirstPartialRight	= Context->NumColumns - 1;

				Context->LastPartialLine	= Context->AnchorLine;
				Context->LastPartialLeft	= 0;
				Context->LastPartialRight	= MAX(0,Context->AnchorColumn - 1);
			}
			else if (Top > Context->AnchorLine)
			{
				DB(kprintf("\033[8H(2)\033[K\n"));

					/* Mouse is below the marker anchor. */

				Context->Direction = 1;

				Context->FirstPartialLine	= Context->AnchorLine;
				Context->FirstPartialLeft	= Context->AnchorColumn;
				Context->FirstPartialRight	= Context->NumColumns - 1;

				Context->LastPartialLine	= Top;
				Context->LastPartialLeft	= 0;
				Context->LastPartialRight	= Left;
			}
			else
			{
				DB(kprintf("\033[8H(3)\033[K\n"));

					/* Mouse is in the same line as the marker anchor. */

				if(Left < Context->AnchorColumn)
				{
						/* Mouse is left of the marker anchor. */

					Context->Direction = -1;

					Context->FirstPartialLeft	= Left;
					Context->FirstPartialRight	= MAX(Left,Context->AnchorColumn - 1);
				}
				else
				{
						/* Mouse is right of the marker anchor. */

					Context->Direction = 1;

					Context->FirstPartialLeft	= Context->AnchorColumn;
					Context->FirstPartialRight	= Left;
				}

				Context->FirstPartialLine	= Context->AnchorLine;
				Context->LastPartialLine	= Context->FirstPartialLine;
				Context->LastPartialLeft	= Context->FirstPartialLeft;
				Context->LastPartialRight	= Context->FirstPartialRight;
			}

			UpdateLineData(Context);
			HighlightAll(Context);
		}
		else
		{
			DB(kprintf("\033[8H\033[K\n"));

				/* Check which line the mouse moved into, with respect to its previous placement. */

			if(Top == Context->CurrentLine)
			{
					/* Mouse is still on the same line. */

				if(Context->Direction > 0)
				{
					if(Left < Context->CurrentColumn)
					{
							/* Mouse moved left (<-). */

						Context->Unhighlight(Context,Context->CurrentLine,Left + 1,Context->CurrentColumn);
					}
					else
					{
							/* Mouse moved right (->). */

						Context->Highlight(Context,Context->CurrentLine,Context->CurrentColumn + 1,Left);
					}

					Context->LastPartialRight = Left;

					if(Context->FirstPartialLine == Context->LastPartialLine)
						Context->FirstPartialRight = Left;
				}
				else
				{
					if(Left < Context->CurrentColumn)
					{
							/* Mouse moved left (<-). */

						Context->Highlight(Context,Context->CurrentLine,Left,Context->CurrentColumn - 1);
					}
					else
					{
							/* Mouse moved right (->). */

						Context->Unhighlight(Context,Context->CurrentLine,Context->CurrentColumn,Left - 1);
					}

					Context->FirstPartialLeft = Left;

					if(Context->FirstPartialLine == Context->LastPartialLine)
						Context->LastPartialLeft = Left;
				}
			}
			else if(Top < Context->CurrentLine)
			{
					/* Mouse is above the previously selected line. */

				if(Context->Direction > 0)
				{
						/* Clear the text selected in the lines below the mouse position. */

					if(Context->LastPartialLine != Context->LastFullLine)
						Context->Unhighlight(Context,Context->LastPartialLine,0,Context->LastPartialRight);

					if(Context->LastFullLine != -1)
					{
						for(i = Context->LastFullLine ; i > Top ; i--)
							Context->Unhighlight(Context,i,0,Context->NumColumns - 1);
					}

						/* Clear the text selected in the new mouse line, but only the text following the new mouse position. */

					Context->Unhighlight(Context,Top,Left + 1,Context->NumColumns - 1);

					Context->LastPartialLine	= Top;
					Context->LastPartialRight	= Left;

					if(Context->LastPartialLine == Context->FirstPartialLine)
						Context->LastPartialLeft = Context->FirstPartialLeft;
					else
						Context->LastPartialLeft = 0;

					if(Context->FirstPartialLine == Context->LastPartialLine)
					{
						Context->FirstPartialLeft	= Context->LastPartialLeft;
						Context->FirstPartialRight	= Context->LastPartialRight;
					}
				}
				else
				{
						/* Fill the first partial line completely. */

					if(Context->FirstPartialLine != Context->FirstFullLine)
					{
						Context->Highlight(Context,Context->FirstPartialLine,0,Context->FirstPartialLeft - 1);

						Context->FirstPartialLeft = 0;

						if(Context->FirstPartialLine == Context->LastPartialLine)
							Context->LastPartialLeft = Context->FirstPartialLeft;
					}

						/* Mark the text selected in the lines below the mouse pointer. */

					for(i = Top + 1 ; i < Context->FirstPartialLine ; i++)
						Context->Highlight(Context,i,0,Context->NumColumns - 1);

					Context->Highlight(Context,Top,Left,Context->NumColumns - 1);

					Context->FirstPartialLine	= Top;
					Context->FirstPartialLeft	= Left;
					Context->FirstPartialRight	= Context->NumColumns - 1;

					if(Context->FirstPartialLine == Context->LastPartialLine)
					{
						Context->LastPartialLeft	= Context->FirstPartialLeft;
						Context->LastPartialRight	= Context->FirstPartialRight;
					}
				}
			}
			else
			{
					/* The mouse is below the last line. */

				if(Context->Direction > 0)
				{
						/* Fill the last partial line completely. */

					if(Context->LastPartialLine != Context->LastFullLine)
					{
						Context->Highlight(Context,Context->LastPartialLine,Context->LastPartialRight + 1,Context->NumColumns - 1);

						Context->LastPartialRight = Context->NumColumns - 1;

						if(Context->LastPartialLine == Context->FirstPartialLine)
							Context->FirstPartialRight = Context->NumColumns - 1;
					}

						/* Fill the lines above the mouse pointer. */

					for(i = Context->LastPartialLine + 1 ; i < Top ; i++)
						Context->Highlight(Context,i,0,Context->NumColumns - 1);

					Context->Highlight(Context,Top,0,Left);

					Context->LastPartialLine	= Top;
					Context->LastPartialLeft	= 0;
					Context->LastPartialRight	= Left;

					if(Context->FirstPartialLine == Context->LastPartialLine)
					{
						Context->FirstPartialLeft	= Context->LastPartialLeft;
						Context->FirstPartialRight	= Context->LastPartialRight;
					}
				}
				else
				{
						/* Clear the first selected line completely. */

					if(Context->FirstPartialLine != Context->FirstFullLine)
						Context->Unhighlight(Context,Context->FirstPartialLine,Context->FirstPartialLeft,Context->NumColumns - 1);

					if(Context->FirstFullLine != -1)
					{
						for(i = Context->FirstFullLine ; i < Top ; i++)
							Context->Unhighlight(Context,i,0,Context->NumColumns - 1);
					}

					Context->Unhighlight(Context,Top,0,Left - 1);

					Context->FirstPartialLine	= Top;
					Context->FirstPartialLeft	= Left;

					if(Context->FirstPartialLine == Context->LastPartialLine)
						Context->FirstPartialRight = Context->LastPartialRight;
					else
						Context->FirstPartialRight = Context->NumColumns - 1;

					if(Context->FirstPartialLine == Context->LastPartialLine)
					{
						Context->LastPartialLeft	= Context->FirstPartialLeft;
						Context->LastPartialRight	= Context->FirstPartialRight;
					}
				}
			}

			UpdateLineData(Context);
		}

		Context->CurrentColumn	= Left;
		Context->CurrentLine	= Top;
	}
}

BOOL
CheckMarkerHighlighting(struct MarkerContext *Context,LONG Line,LONG *Left,LONG *Right)
{
	if(Line < Context->FirstPartialLine || Line > Context->LastPartialLine)
		return(FALSE);
	else
	{
		if(Line == Context->FirstPartialLine)
		{
			*Left	= Context->FirstPartialLeft;
			*Right	= Context->FirstPartialRight;
		}
		else if (Line == Context->LastPartialLine)
		{
			*Left	= Context->LastPartialLeft;
			*Right	= Context->LastPartialRight;
		}
		else
		{
			*Left	= 0;
			*Right	= Context->NumColumns - 1;
		}

		return(TRUE);
	}
}

VOID
TransferMarker(struct MarkerContext *Context,ULONG Qualifier)
{
	if(Context->FirstPartialLine != -1 && Context->LastPartialLine != -1)
	{
		APTR UserData;

		if(UserData = Context->TransferStartStop(Context,NULL,Qualifier))
		{
			if(Context->FirstPartialLine == Context->LastPartialLine)
				Context->Transfer(Context,Context->FirstPartialLine,Context->FirstPartialLeft,Context->FirstPartialRight,Context->Put,UserData);
			else
			{
				BOOL Ok;
				LONG i;

				for(i = Context->FirstPartialLine ; i <= Context->LastPartialLine ; i++)
				{
					if(i == Context->FirstPartialLine)
						Ok = Context->Transfer(Context,i,Context->FirstPartialLeft,Context->FirstPartialRight,Context->PutLine,UserData);
					else if (i == Context->LastPartialLine)
					{
						if(Context->LastPartialLeft == 0 && Context->LastPartialRight == Context->NumColumns - 1)
							Ok = Context->Transfer(Context,i,Context->LastPartialLeft,Context->LastPartialRight,Context->PutLine,UserData);
						else
							Ok = Context->Transfer(Context,i,Context->LastPartialLeft,Context->LastPartialRight,Context->Put,UserData);
					}
					else
						Ok = Context->Transfer(Context,i,0,Context->NumColumns - 1,Context->PutLine,UserData);

					if(!Ok)
						break;
				}
			}

			Context->TransferStartStop(Context,UserData,0);
		}
	}
}

BOOL
SetWordMarker(
	struct MarkerContext *Context,
	LONG FirstVisibleLine,
	LONG NumVisibleLines,
	LONG NumLines,

	LONG FirstVisibleColumn,
	LONG NumVisibleColumns,
	LONG NumColumns)
{
	LONG Left,Top,WordLeft,WordRight;

	Context->FirstVisibleLine	= FirstVisibleLine;
	Context->NumVisibleLines	= NumVisibleLines;
	Context->NumLines			= NumLines;

	Context->FirstVisibleColumn	= FirstVisibleColumn;
	Context->NumVisibleColumns	= NumVisibleColumns;
	Context->NumColumns			= NumColumns;

	Context->AskPosition(Context,&Left,&Top,MARKERASK_Clipped);

	Left	+= Context->FirstVisibleColumn;
	Top		+= Context->FirstVisibleLine;

	if(!Context->PickWord(Context,Left,Top,&WordLeft,&WordRight))
		return(FALSE);
	else
	{
		Context->AnchorColumn		= Left;
		Context->AnchorLine			= Top;

		Context->CurrentColumn		= Left + WordLeft - WordRight;
		Context->CurrentLine		= Top;

		Context->FirstPartialLine	= Top;
		Context->FirstPartialLeft	= WordLeft;
		Context->FirstPartialRight	= WordRight;

		Context->LastPartialLine	= Context->FirstPartialLine;
		Context->LastPartialLeft	= Context->FirstPartialLeft;
		Context->LastPartialRight	= Context->FirstPartialRight;

		Context->Direction			= 1;

		UpdateLineData(Context);
		HighlightAll(Context);

		return(TRUE);
	}
}

VOID
SetMarker(
	struct MarkerContext *Context,
	LONG FirstVisibleLine,
	LONG NumVisibleLines,
	LONG NumLines,

	LONG FirstVisibleColumn,
	LONG NumVisibleColumns,
	LONG NumColumns,

	LONG Line,
	LONG WordLeft,
	LONG WordRight)
{
	Context->FirstVisibleLine	= FirstVisibleLine;
	Context->NumVisibleLines	= NumVisibleLines;
	Context->NumLines			= NumLines;

	Context->FirstVisibleColumn	= FirstVisibleColumn;
	Context->NumVisibleColumns	= NumVisibleColumns;
	Context->NumColumns			= NumColumns;

	Context->AnchorColumn		= WordLeft;
	Context->AnchorLine			= Line;

	Context->CurrentColumn		= WordRight;
	Context->CurrentLine		= Line;

	Context->FirstPartialLine	= Line;
	Context->FirstPartialLeft	= WordLeft;
	Context->FirstPartialRight	= WordRight;

	Context->LastPartialLine	= Context->FirstPartialLine;
	Context->LastPartialLeft	= Context->FirstPartialLeft;
	Context->LastPartialRight	= Context->FirstPartialRight;

	Context->Direction			= 1;

	UpdateLineData(Context);
	HighlightAll(Context);
}

VOID
SelectAllMarker(
	struct MarkerContext *Context,
	LONG FirstVisibleLine,
	LONG NumVisibleLines,
	LONG NumLines,

	LONG FirstVisibleColumn,
	LONG NumVisibleColumns,
	LONG NumColumns,

	LONG FirstLeft,
	LONG FirstLine,
	LONG LastRight,
	LONG LastLine)
{
	Context->FirstVisibleLine	= FirstVisibleLine;
	Context->NumVisibleLines	= NumVisibleLines;
	Context->NumLines			= NumLines;

	Context->FirstVisibleColumn	= FirstVisibleColumn;
	Context->NumVisibleColumns	= NumVisibleColumns;
	Context->NumColumns			= NumColumns;

	Context->AnchorColumn		= FirstLeft;
	Context->AnchorLine			= FirstLine;

	Context->CurrentColumn		= LastRight;
	Context->CurrentLine		= LastLine;

	Context->FirstPartialLine	= FirstLine;
	Context->FirstPartialLeft	= FirstLeft;
	Context->FirstPartialRight	= NumColumns - 1;

	Context->LastPartialLine	= LastLine;
	Context->LastPartialLeft	= 0;
	Context->LastPartialRight	= LastRight;

	Context->Direction			= 1;

	UpdateLineData(Context);
	HighlightAll(Context);
}

BOOL
CheckMarker(struct MarkerContext *Context)
{
	if(Context)
	{
		if(Context->FirstPartialLine != -1 && Context->LastPartialLine != -1)
			return(TRUE);
	}

	return(FALSE);
}
