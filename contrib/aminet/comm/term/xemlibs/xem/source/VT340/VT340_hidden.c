#include "ownincs/VT340console.h"




STATIC BOOL ValidDimensions(struct VT340Console *con, struct Rectangle *rect)
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

	xMax--;
	yMax--;

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


VOID mySetRast(struct VT340Console *con)
{
	myRectFill(con, 0, 0, con->border.Width-1, con->border.Height-1, BACKGROUND_PEN);
}


VOID myScrollRaster(struct VT340Console *con, WORD dx, WORD dy, WORD xMin, WORD yMin, WORD xMax, WORD yMax)
{
	struct Rectangle rect;

	rect.MinX = xMin + con->border.Left;
	rect.MinY = yMin + con->border.Top;
	rect.MaxX = xMax + con->border.Left;
	rect.MaxY = yMax + con->border.Top;

	if(ValidDimensions(con, &rect) == FALSE)
		return;

	if(con->mono == FALSE)
		SetBPen(con->rp, BACKGROUND_PEN);

	ScrollRaster(con->rp, dx, dy, rect.MinX, rect.MinY, rect.MaxX, rect.MaxY);
	
	if(con->mono == FALSE)
		SetBPen(con->rp, con->bpen);
}


VOID myRectFill(struct VT340Console *con, WORD xMin, WORD yMin, WORD xMax, WORD yMax, BYTE fgPen)
{
	struct Rectangle rect;

	rect.MinX = xMin + con->border.Left;
	rect.MinY = yMin + con->border.Top;
	rect.MaxX = xMax + con->border.Left;
	rect.MaxY = yMax + con->border.Top;

	if(ValidDimensions(con, &rect) != FALSE)
	{
		if(fgPen != -1)
			SetAPen(con->rp, fgPen);

		RectFill(con->rp, rect.MinX, rect.MinY, rect.MaxX, rect.MaxY);

		if(fgPen != -1)
			SetAPen(con->rp, con->apen);
	}
}

VOID myMove_Text(struct VT340Console *con, UWORD xPos, UWORD yPos, STRPTR string, UWORD count)
{
	struct Rectangle rect;

	rect.MinX = ((xPos-1)<<3) + con->border.Left;
	rect.MinY = ((yPos * FONT_YSIZE) - (FONT_YSIZE - BOTLINE)) + con->border.Top;
	rect.MaxX = con->border.Left + con->border.Width;
	rect.MaxY = con->border.Top + con->border.Height;

	if(ValidDimensions(con, &rect) != FALSE)
	{
		Move(con->rp, rect.MinX, rect.MinY);
		Text(con->rp, string, count);
	}
}


VOID myBltBitMapRastPort(struct VT340Console *con, struct BitMap *srcBitMap, LONG xSrc, LONG ySrc, LONG xDest, LONG yDest, LONG xSize, LONG ySize, ULONG minterm)
{
	struct Rectangle rect;

	rect.MinX = xDest + con->border.Left;
	rect.MinY = yDest + con->border.Top;
	rect.MaxX = rect.MinX + xSize;
	rect.MaxY = rect.MinY + ySize;

	if(ValidDimensions(con, &rect) != FALSE)
		BltBitMapRastPort(srcBitMap, xSrc, ySrc, con->rp, rect.MinX, rect.MinY, rect.MaxX - rect.MinX, rect.MaxY - rect.MinY, minterm);
}

/* end of source-code */
