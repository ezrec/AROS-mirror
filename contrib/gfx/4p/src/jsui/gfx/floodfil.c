/*
 * floodfil.c
 *
 *   A non-recursing flood fill algorithm
 *
 *   Written by Patrick Stein (pat@mail.csh.rit.edu) 2001.08.27
 *   Integrated into 4P on 2001.09.16  :D
 *
 *      Thanks, Pat!  YOU ROCK!  ;)
 *
 *   Small modifications have been made to make it work with 4P
 *   by Jerry Lawrence
 */

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2001 Scott "Jerry" Lawrence, Patrick Stein
 *                    jsl.4p@absynth.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/***

<html>
<h2>Flood Fill</h2>

<p>To do a flood fill of a region, the first that we need to do is
to create some data structure to store the region.  I am going to
use a simple linked list where each node contains information about
a single, horizontal line that is in the area of interest.<p>

<pre>
    typedef struct Extent {
	unsigned int x1;
	unsigned int x2;
	unsigned int yy;

	unsigned int checkedNeighbors;

	struct Extent* next;
    } Extent;
</pre>

<p>This structure stores the two horizontal coordinates and the
vertical coordinate of the horizontal line in the area of interest.
It also stores a flag to see whether we have already checked the
space above and below this line.  And, it keeps track of the next
item in the linked list.</p>

<p>Now, here is some pseudo-code for flood filling an area, starting
with a given point <code>(sx,sy)</code>.</p>

<p>First, we will prime the pump.</p>
<pre>
    Empty the list of Extents.

    Keep going left until you hit a border to find x1.
    Keep going right until you hit a border to find x2.

    Add (x1,x2,sy) to your list of Extents (with the
	``checkedNeighbors'' field set to false).
</pre>

<p>Borders are places where two adjacent pixels differ signficantly
enough.  For most purposes, differing at all is differing enough.</p>

<p>Then, we will keep exploring regions that are above or below
regions we have already found until we have checked all of the
neighbors of all regions of interest.
The <code>Check pixels in the neighboring row</code> here is
broken out below.</p>
<pre>
    while there are extents with checkedNeighbors set false
	Pick an Extent with unchecked neighbors.

	Set yy to Extent's yy

	Set sx to Extent's x1
	Set sy to Extent's yy-1
	Check pixels in the neighboring row

	Set sx to Extent's x1
	Set sy to Extent's yy+1
	Check pixels in the neighboring row

	Set the Extent's ``checkedNeighbors'' to true.
</pre>

<p>To check pixels in the neighboring row, we have to loop
through the whole horizontal extents of the region we are
trying to check.  But, we can fast-forward through parts
that we already know are in the region of interest.</p>
<pre>
    while sx is less than or equal to Extent's x2
	If pixel at (sx,sy) matches (sx,yy)
	    If (sx,sy) is already accounted for in the Extents list
		Set x2 from the x2 in the Extent containing (sx,sy)
	    Otherwise
		Go left (in sy's row) until you hit a border to find x1.
		Go right (in sy's row) until you hit a border to find x2.
		Add (x1,x2,sy) to your list of Extents (with the
		``checkedNeighbors''
	    Set sx to x2 + 1

	Increment sx
</pre>

<p>The reason we can set <code>sx</code> to <code>x2+1</code> and
still increment <code>sx</code> is because we know that part of
the region of interest ends at x2.  This means that <code>x2+1</code>
is *not* in the region of interest.  The next potential pixel for
the region of interest is <code>x2+2</code>.</p>

<p>And, that does it for finding the region.  Now, you can go
through and do what you want with those regions.  You may wish to
keep the list of Extents sorted so that it is a little quicker to
look for things in the list and a little simpler to use the list
once you're done with it.</p>

<p>This algorithm will never check any pixels that are outside of
the border region.  It will check isolated pixels inside the region
of interest four times, but it will only check true border pixels
two times at the most.  And, it can glide over areas that have
already been found.  There are algorithms that do fewer checks but
require more memory.  I think this one is a good trade off.</p>

</html>

***/

#include <stdlib.h>
#include "jsui.h"

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct PatFill_Extents {
    unsigned int x1;
    unsigned int x2;
    unsigned int yy;

    BOOL checked;

    struct PatFill_Extents* next;
} PatFill_Extents;

typedef int PatFill_Pixel;
typedef BOOL (*PatFill_MatchingFunction)( PatFill_Pixel* aa, PatFill_Pixel* bb );



static BOOL
PatFill_Equal( PatFill_Pixel* aa, PatFill_Pixel* bb )
{
    if (!aa || !bb) return FALSE;

    return (*aa == *bb);
}


static void
PatFill_FindExtents(
	unsigned int sx, 
	PatFill_MatchingFunction match,
	PatFill_Pixel* row,
	unsigned int width,
	unsigned int* x1,
	unsigned int* x2
    )
{
    unsigned int xx = sx;
    PatFill_Pixel* prev;
    PatFill_Pixel* cur;

    if (!match || !row || !x1 || !x2) return;

    prev = &row[ sx ];
    cur = &prev[ -1 ];


    while ( cur >= row && (*match)( cur, prev ) ) {
	prev = cur;
	--cur;
	--xx;
    }

    *x1 = xx;

    prev = &row[ sx ];
    cur = &prev[ 1 ];

    xx = sx;

    while ( cur < &row[ width ] && (*match)( cur, prev ) ) {
	prev = cur;
	++cur;
	++xx;
    }

    *x2 = xx;
}


static PatFill_Extents*
PatFill_AddExtent( 
        unsigned int x1,
        unsigned int x2,
        unsigned int yy,
        PatFill_Extents* info )
{
    PatFill_Extents* nn = (PatFill_Extents *)malloc(sizeof(PatFill_Extents));

    nn->x1 = x1;
    nn->x2 = x2;
    nn->yy = yy;
    nn->checked = FALSE;
    nn->next = info;

    return nn;
}


static PatFill_Extents*
PatFill_FindUnchecked( PatFill_Extents* info )
{
    while ( info != 0 && info->checked ) {
	info = info->next;
    }

    return info;
}


static BOOL
PatFill_AccountedFor(
	unsigned int xx,
        unsigned int yy,
        PatFill_Extents* info,
	unsigned int* nx1,
        unsigned int* nx2
    )
{
    if (!nx1 || !nx2) return FALSE;

    while ( info != 0 ) {
	if ( info->yy == yy && info->x1 <= xx && info->x2 >= xx ) {
	    *nx1 = info->x1;
	    *nx2 = info->x2;
	    return TRUE;
	}

	info = info->next;
    }

    return FALSE;
}


static PatFill_Extents*
PatFill_CheckNeighborLine(
	PatFill_Extents* info,
	unsigned int x1, unsigned int x2, unsigned int yy,
	PatFill_MatchingFunction match,
	PatFill_Pixel* curLine,
        PatFill_Pixel* neighbor, 
        unsigned int width
    )
{
    unsigned int ii;
    for ( ii=x1; ii <= x2; ++ii ) {
	BOOL mm = (*match)( &curLine[ ii ], &neighbor[ ii ] );
	
	if ( mm ) {
	    unsigned int nx1;
	    unsigned int nx2;

	    if ( !PatFill_AccountedFor( ii, yy, info, &nx1, &nx2 ) ) {
		PatFill_FindExtents( ii, match, neighbor, width, &nx1, &nx2 );
		info = PatFill_AddExtent( nx1, nx2, yy, info );
	    }

	    ii = nx2 + 1;
	}
    }

    return info;
}


static PatFill_Extents*
PatFill_FindArea(
	unsigned int sx, unsigned int sy,
	PatFill_MatchingFunction match,
	PatFill_Pixel* base, unsigned int width, unsigned int height
    )
{
    unsigned int x1;
    unsigned int x2;
    PatFill_Extents* info = NULL;

    if (!match || !base) return NULL;

    PatFill_FindExtents( sx, match, &base[ sy * width ], width, &x1, &x2 );

    info = PatFill_AddExtent( x1, x2, sy, info );

    for (;;) {
	PatFill_Extents* ptr = PatFill_FindUnchecked( info );

	if ( ptr == 0 ) {
	    break;
	}

	x1 = ptr->x1;
	x2 = ptr->x2;
	sy = ptr->yy;

	if ( sy > 0 ) {
	    info = PatFill_CheckNeighborLine(
		    info,
		    x1, x2, sy-1,
		    match,
		    &base[ sy * width ], &base[ (sy-1) * width ], width
		);
	}

	if ( sy+1 < height ) {
	    info = PatFill_CheckNeighborLine(
		    info,
		    x1, x2, sy+1,
		    match,
		    &base[ sy * width ], &base[ (sy+1) * width ], width
		);
	}

	ptr->checked = TRUE;
    }

    return info;
}

void
primitive_floodfill(
        IPAGE * dest,
        PEN * pen,
        int sx, int sy
)
{
    int dx1, dy1, dx2, dy2;
    unsigned int ii;
    PatFill_Extents* info = NULL;
    PatFill_Extents* old = NULL;

    if (!dest || !pen) return;

    dx1 = dest->w;
    dy1 = dest->h;
    dx2 = 0;
    dy2 = 0;

    info = PatFill_FindArea( sx, sy, PatFill_Equal, dest->pimage, dest->w, dest->h );

    while ( info != 0 ) {
	PatFill_Pixel* ptr = &dest->pimage[ info->yy * dest->w ];

	for ( ii=info->x1; ii <= info->x2; ++ii ) {
	    /* now set the pixels .. use a callback here eventually. */
	    ptr[ ii ] = pen->Icolor;
	}
        dx1 = MIN(info->x1, dx1);
        dx1 = MIN(info->x2, dx1);
        
        dx2 = MAX(info->x1, dx2);
        dx2 = MAX(info->x2, dx2);
        
        dy1 = MIN(info->yy, dy1);
        dy2 = MAX(info->yy, dy2);

	old = info;
	info = info->next;
	free(old);
    }

    Page_DR_Dirtify(dest, dx1, dy1, dx2, dy2);
}

