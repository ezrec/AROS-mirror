/* Copyright 2000 Kjetil S. Matheussen

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */


/*******************************************************************

   OVERVIEW

     This file calculate all sort of nodelines by offering very
     a general function.

     It can be speeded up by haveing a more general struct that
     is sent between the functions instead of reorganizing the
     stack all the time by having the variables as arguments all
     the time. (MakeNodeLines() is called very often)
*******************************************************************/



#include "nsmtracker.h"
#include "placement_proc.h"
#include "realline_calc_proc.h"
#include "nodelines.h"

#include "nodelines_proc.h"




struct TTVector{
	float x1,x2;
	float y1,y2;
};
typedef struct TTVector TVector;


/**************************************************************
  VARIABLES

    The procedures below uses various forms of x and y values.
    This is an explanation on what they are used for.

    TVector=(x1,y1),(x2,y2)       - Vector. Used for calculation of the
                                    other values.
    float t_y1,t_y2               - Start and end y values within a realline,
                                    in the form of (counter/dividor)
                                    represented as float.
    int (u_x1,u_y1),(u_x2,u_y2)   - Start and end values for a line
                                    on a trackrealline. The values are relative
                                    to the top left corner of the trackrealline.

**************************************************************/





/**************************************************************
  FUNCTION
    Returns the relative X position, calculated from
    the vector (x,maxx).
**************************************************************/
static __inline int GetRelXPos(
	float x,
	float maxx,
	int arealength
){
	float x2;

	x2=x*arealength/maxx;

	return (int) (x2+0.5);
}



/*************************************************************
  FUNCTION
    Return the x value for the y value 'y' calculated from
    the vector tvector.
*************************************************************/
static __inline float GetXFromVector(
	float y,
	TVector *tvector
){
	return (float)(
		(y - tvector->y1)/
		(tvector->y2 - tvector->y1) *
		(tvector->x2 - tvector->x1)

		+tvector->x1
	);
}


/*************************************************************
  FUNCTION
    Return the x value for the y value 'y' calculated from
    the vector (x1,y1)-(x2,y2) encapsulated by the
    arealength.
*************************************************************/
static __inline int GetSubTrackXFromVector(
	float maxx,int areallength,
	float y,
	TVector *tvector
){
	return
		GetRelXPos(
			GetXFromVector(y,tvector),
			maxx,
			areallength
		);
}



/**************************************************************
  FUNCTION
    Fills in graphical line-data for 'realline'.
**************************************************************/
void FillInLineRealLine(
	struct Tracker_Windows *window,
	struct WBlocks *wblock,
	int realline,
	TVector *tvector,
	float t_y1,float t_y2,
	int u_y1,int u_y2,
	float maxx,
	int startx,int endx,
	void *extrainfo,
	void (*ReturnNodeLineData)(
		struct Tracker_Windows *window,
		struct WBlocks *wblock,
		void *extrainfo,
		int firstlast,
		int realline,
		int u_y1,int u_y2,int u_x1,int u_x2
	),
	int firstlast
){
	int u_x1,u_x2;
	u_x1=startx+GetSubTrackXFromVector(maxx,endx-startx,t_y1,tvector);
	u_x2=startx+GetSubTrackXFromVector(maxx,endx-startx,t_y2,tvector);
	(*ReturnNodeLineData)(
		window,
		wblock,
		extrainfo,
		firstlast,
		realline,
		u_y1,u_y2,
		u_x1,u_x2
	);
}

/******************************************************************
  FUNCTION
    Fill in graphical line-data for the reallines between/and
    start_realline and end_realline.
**************************************************************/
void FillInVelRealLines(
	struct Tracker_Windows *window,
	struct WBlocks *wblock,
	TVector *tvector,
	float maxx,
	int startx,int endx,
	int start_realline,
	int end_realline,
	void *extrainfo,
	void (*ReturnNodeLineData)(
		struct Tracker_Windows *window,
		struct WBlocks *wblock,
		void *extrainfo,
		int firstlast,
		int realline,
		int u_y1,int u_y2,int u_x1,int u_x2
	),
	int firstlast
){
	int lokke;
	int nfirstlast=NODELINE_NOTFIRSTORLAST;
	float t_y1,t_y2;
	struct LocalZooms *localzoom;

	for(lokke=start_realline;lokke<=end_realline;lokke++){

		localzoom=wblock->reallines[lokke];
		t_y1=GetfloatFromLineCounterDividor(&localzoom->l.p);
		localzoom=wblock->reallines[lokke+1];
		t_y2=GetfloatFromLineCounterDividor(&localzoom->l.p);

		if(firstlast==NODELINE_LAST && lokke==end_realline) nfirstlast=NODELINE_LAST;
		FillInLineRealLine(
			window,
			wblock,
			lokke,
			tvector,
			t_y1,t_y2,
			0,window->fontheight-1,
			maxx,
			startx,endx,
			extrainfo,
			ReturnNodeLineData,
			nfirstlast
		);
	}

}



/**********************************************************************
  FUNCTION
    Given the line,counter and dividor for start and end of a nodeline;
    Make nodelines that spans this area.
  INPUTS
    window             - window
    wblock             - wblock
    p1                 - Start placement
    p2                 - End placement
    x1                 - Start x value    (this is not a graphical x, but a vector)
    x2                 - End x value       -------------- "" ---------------------
    minx               - minimum x value   -------------- "" ---------------------
    maxx               - maximum x value   -------------- "" ---------------------
    startx             - Graphical start x value
    endx               - Graphical end x value
    extrainfo          - Information sent with the ReturnNodeLineData function
    ReturnNodeLineData - When a nodeline has been calculated, this function
                         is called. u_y1,u_y2,u_x1,u_x2 is graphical coordinates.
                         For the parameter 'firstlast', check out nodelines.h.
**********************************************************************/
void MakeNodeLines(
	struct Tracker_Windows *window,
	struct WBlocks *wblock,
	Place *p1,
	Place *p2,
	float x1,float x2,
	float minx,float maxx,
	int startx,int endx,
	void *extrainfo,
	void (*ReturnNodeLineData)(
		struct Tracker_Windows *window,
		struct WBlocks *wblock,
		void *extrainfo,
		int firstlast,
		int realline,
		int u_y1,int u_y2,int u_x1,int u_x2
	)
){
	TVector tvector;
	int realline1,realline2;
	struct LocalZooms *localzoom,*localzoom2;
	float t_y1,t_y2,t_y2_2;
	int firstlast;

	/* Convert x1,x2 and maxx to delta values to minx. */
	x1-=minx;
	x2-=minx;
	maxx-=minx;

	tvector.y1=GetfloatFromLineCounterDividor(p1);
	tvector.y2=GetfloatFromLineCounterDividor(p2);
	tvector.x1=x1;
	tvector.x2=x2;

	realline1=FindRealLineFor(wblock,p1->line,p1);
	realline2=FindRealLineFor(wblock,max(realline1,p2->line),p2);

	t_y1=tvector.y1;
	t_y2=tvector.y2;

	if(realline2==realline1){
		FillInLineRealLine(
			window,
			wblock,
			realline1,
			&tvector,
			t_y1,t_y2,
			FindSubRealLine(window,wblock,realline1,p1),
			FindSubRealLine(window,wblock,realline2,p2),
			maxx,
			startx,endx,
			extrainfo,
			ReturnNodeLineData,
			NODELINE_FIRSTANDLAST
		);		
		return;
	}

	localzoom=wblock->reallines[realline1+1];
	t_y2=GetfloatFromLineCounterDividor(&localzoom->l.p);

	localzoom2=wblock->reallines[realline2];
	t_y2_2=GetfloatFromLineCounterDividor(&localzoom2->l.p);

	firstlast=NODELINE_FIRST;
	if(realline2<=realline1+1 && t_y2_2==tvector.y2) firstlast=NODELINE_FIRSTANDLAST;

	FillInLineRealLine(
		window,
		wblock,
		realline1,
		&tvector,
		t_y1,t_y2,
		FindSubRealLine(window,wblock,realline1,p1),
		window->fontheight-1,
		maxx,
		startx,endx,
		extrainfo,
		ReturnNodeLineData,
		firstlast
	);		

	if(t_y2_2==tvector.y2) firstlast=NODELINE_LAST;
	if(realline2>realline1+1)
		FillInVelRealLines(
			window,
			wblock,
			&tvector,
			maxx,
			startx,endx,
			realline1+1,
			realline2-1,
			extrainfo,
			ReturnNodeLineData,
			firstlast
		);


	localzoom2=wblock->reallines[realline2];
	t_y2=GetfloatFromLineCounterDividor(&localzoom2->l.p);

	if(t_y2==tvector.y2) return;

	t_y1=t_y2;

	t_y2=tvector.y2;

	FillInLineRealLine(
		window,
		wblock,
		realline2,
		&tvector,
		t_y1,t_y2,
		0,
		FindSubRealLine(window,wblock,realline2,p2),
		maxx,
		startx,endx,
		extrainfo,
		ReturnNodeLineData,
		NODELINE_LAST
	);		

}
	
















