#include "ownincs/BBSconsole.h"


STATIC BOOL ValidDimensions(struct BBSConsole *con, struct Rectangle *rect, BOOL adjust)
{
	struct Window *win;
	WORD xMin, yMin, xMax, yMax;

	if(con->isReady == FALSE)
		return(FALSE);

	if(con->displayInactive != FALSE)
		return(FALSE);

	win	= con->io->xem_window;
	xMin	= win->BorderLeft;
	yMin	= win->BorderTop;
	xMax	= win->Width - win->BorderRight;
	yMax	= win->Height - win->BorderBottom;

	if(adjust != FALSE)	/* ScrollRaster(), RectFill() */
	{
		xMax--;
		yMax--;
	}

/************/

	if(rect->MinX >= rect->MaxX)
		return(FALSE);

	if(rect->MinY >= rect->MaxY)
		return(FALSE);

/************/

	if(rect->MinX >= xMax)
		return(FALSE);

	if(rect->MinY >= yMax)
		return(FALSE);

/************/

	if(rect->MinX < xMin)
		rect->MinX = xMin;

	if(rect->MinY < yMin)
		rect->MinY = yMin;

/************/

	if(rect->MaxX > xMax)
		rect->MaxX = xMax;

	if(rect->MaxY > yMax)
		rect->MaxY = yMax;

/************/


	return(TRUE);
}


VOID mySetRast(struct BBSConsole *con)
{
	myRectFill(con, 0, 0, con->border.Width-1, con->border.Height-1, BACKGROUND_PEN);
}


VOID myScrollRaster(struct BBSConsole *con, WORD dx, WORD dy, WORD xMin, WORD yMin, WORD xMax, WORD yMax)
{
	struct Rectangle rect;

	rect.MinX = xMin + con->border.Left;
	rect.MinY = yMin + con->border.Top;
	rect.MaxX = xMax + con->border.Left;
	rect.MaxY = yMax + con->border.Top;

	if(ValidDimensions(con, &rect, TRUE) == FALSE)
		return;

	if(con->mono == FALSE)
		SetBPen(con->rp, BACKGROUND_PEN);

	ScrollRaster(con->rp, dx, dy, rect.MinX, rect.MinY, rect.MaxX, rect.MaxY);
	
	if(con->mono == FALSE)
		SetBPen(con->rp, con->bpen);
}


VOID myRectFill(struct BBSConsole *con, WORD xMin, WORD yMin, WORD xMax, WORD yMax, UBYTE fgPen)
{
	struct Rectangle rect;

	rect.MinX = xMin + con->border.Left;
	rect.MinY = yMin + con->border.Top;
	rect.MaxX = xMax + con->border.Left;
	rect.MaxY = yMax + con->border.Top;

	if(ValidDimensions(con, &rect, TRUE) != FALSE)
	{
		if(fgPen != -1)
			SetAPen(con->rp, fgPen);

		RectFill(con->rp, rect.MinX, rect.MinY, rect.MaxX, rect.MaxY);

		if(fgPen != -1)
			SetAPen(con->rp, con->apen);
	}
}


VOID myMove_Text(struct BBSConsole *con, UWORD xPos, UWORD yPos, STRPTR string, UWORD count)
{
	struct Rectangle rect;

	rect.MinX = ((xPos-1) * XSIZE) + con->border.Left;
	rect.MinY = (((yPos-1) * YSIZE) + BOTLINE) + con->border.Top;
	rect.MaxX = con->border.Left + con->border.Width;
	rect.MaxY = con->border.Top + con->border.Height;

	if(ValidDimensions(con, &rect, FALSE) != FALSE)
	{
		Move(con->rp, rect.MinX, rect.MinY);
		Text(con->rp, string, count);
	}
}


/* end of source-code */
