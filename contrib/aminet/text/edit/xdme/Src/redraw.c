/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	This contains all functionality that is needed to refresh
	part of the display.

******************************************************************************/

/**************************************
		Includes
**************************************/
#include <defs.h>
#define MYDEBUG     0
#include "debug.h"


/**************************************
	    Globale Variable
**************************************/


/**************************************
      Interne Defines & Strukturen
**************************************/
#define SWAP(a,b)   (void)((a)^=(b),(b)^=(a),(a)^=(b))
#define MIN(a,b)    ((a) <= (b) ? (a) : (b))
#define MAX(a,b)    ((a) >= (b) ? (a) : (b))
#define CLIP(l,m,h) (((m) < (l)) ? (l) : (((m) > (h)) ? (h) : (m)))

#define BF_LINESWAP	    1L
#define BF_COLUMNSWAP	    2L

/* We note all areas that need to be drawn/erased in this structure.

    (startline<=endline && startcol<=endcol) == TRUE !
*/

#define NO_INTERSECT	    0
#define TOP_CLIP	    0x0001
#define BOTTOM_CLIP	    0x0010
#define LEFT_CLIP	    0x0100
#define RIGHT_CLIP	    0x1000
#define TOP_INTERSECT	    0x0002
#define BOTTOM_INTERSECT    0x0020
#define LEFT_INTERSECT	    0x0200
#define RIGHT_INTERSECT     0x2000

#define IS_EMPTY	    0
#define NOT_EMPTY	    1
#define MEMORY_ERROR	    2

struct RefreshRegion
{
    MLIST list; 	/* linked list of regions */
};

struct rect
{
    Line   sl;		/* line where the region starts in */
    Column sc;		/* and the column */
    Line   el;		/* same for end-pos */
    Column ec;
};


struct MyRegion
{
    MNODE  node;	/* for linking into a list */
    struct rect rect;
};


/**************************************
	    Interne Variable
**************************************/


/**************************************
	   Interne Prototypes
**************************************/
static struct RefreshRegion * new_region (void);
static void dispose_region (struct RefreshRegion *);
static UWORD do_clip (struct MyRegion *, struct rect *);
static int and_rect_region (struct RefreshRegion *, struct rect *);
static BOOL add_rect (struct RefreshRegion *, struct rect *);
static int clear_rect_region (struct RefreshRegion *, struct rect *);
static void refresh_region (RP *, struct RefreshRegion *, int);
static void print_region (struct RefreshRegion *);


/*****************************************************************************

    NAME
	redraw_block

    PARAMETER
	BOOL   force;		Shall I care for the difference between
				the fields in the ActualBlock or just
				redraw the block in the limits specified
				in the parameters ?
	Line   start_line;
	Column start_column;
	Line   end_line;
	Column end_column;

    RETURN
	void

    DESCRIPTION
	This function draws a block according to the type. The parameter
	force has a special meaning. If it is TRUE, you force a redraw.
	The block is newly drawn. In this case, the lines specify the
	clip-rect of text that is to be redrawn.
	    If it is FALSE, the positions specify the new start- and
	end-positions for the block. The routine gurantees that the
	start-position of the block is before the end-position after the
	call. It is valid to call the routine with -1/-1 for one position.
	In this case, you just want to move one position without touching
	the other. The routine also takes care for flipping the block.

    NOTES
	- If force is FALSE, you MUST NOT change more than one position at
	  a time. You can exchange both and change one, though.
	- The block is only redrawn, if the actual editor is the editor
	  with the block and drawing in that editor is ok !
	- After the call, the new positions are written into the ActualBlock-
	  struct if force is FALSE. It is guranteed, that the start-position
	  is before the end-position, no matter how they were when the routine
	  was called.

******************************************************************************/

static void print_region (struct RefreshRegion * region)
{
    struct MyRegion * ptr;
    int t;

    for (t=0,ptr=GetHead(&region->list); ptr; ptr=GetSucc(&ptr->node),t++)
    {
	D(bug("Node %ld: (%ld/%ld) - (%ld/%ld)\n",
	    t, ptr->rect.sl, ptr->rect.sc, ptr->rect.el, ptr->rect.ec));
    }
} /* print_region */


static struct RefreshRegion * new_region (void)
{
    struct RefreshRegion * new;

    if (new = AllocMem (sizeof(struct RefreshRegion), 0))
    {
	NewList ((struct List *)&new->list);
    }

    return (new);
} /* new_region */


static void dispose_region (struct RefreshRegion * region)
{
    struct MyRegion * ptr, * next;

    for (ptr=GetHead(&region->list); ptr; ptr=next)
    {
	next = GetSucc (&ptr->node);

	FreeMem (ptr, sizeof(struct MyRegion));
    }

    FreeMem (region, sizeof(struct RefreshRegion));
} /* dispose_region */


static UWORD do_clip (struct MyRegion * region, struct rect * rect)
{
    UWORD test = NO_INTERSECT;

    if (!(  (region->rect.sl > rect->el) || (region->rect.el < rect->sl) ||
	    (region->rect.sc > rect->ec) || (region->rect.ec < rect->sc) ) )
    {
	if (region->rect.sl < rect->sl)
	    test |= TOP_CLIP;

	if (region->rect.el > rect->el)
	    test |= BOTTOM_CLIP;

	if (region->rect.sc < rect->sc)
	    test |= LEFT_CLIP;

	if (region->rect.ec > rect->ec)
	    test |= RIGHT_CLIP;

	if (region->rect.sl <= rect->el)
	    test |= TOP_INTERSECT;

	if (region->rect.el >= rect->sl)
	    test |= BOTTOM_INTERSECT;

	if (region->rect.sc <= rect->ec)
	    test |= LEFT_INTERSECT;

	if (region->rect.ec >= rect->sc)
	    test |= RIGHT_INTERSECT;
    }

    /* D(bug("clip %04lx\n", test)); */

    return (test);
} /* do_clip */


/* clip regions */

static int and_rect_region (struct RefreshRegion * region, struct rect * rect)
{
    struct MyRegion * ptr, * next;
    UWORD status;

    for (ptr=GetHead(&region->list); ptr; ptr=next)
    {
	UWORD common;

	next = GetSucc (&ptr->node);

	if ((common = do_clip (ptr, rect)) != NO_INTERSECT)
	{
	    if (common & TOP_CLIP)
		ptr->rect.sl = rect->sl;

	    if (common & BOTTOM_CLIP)
		ptr->rect.el = rect->el;

	    if (common & LEFT_CLIP)
		ptr->rect.sc = rect->sc;

	    if (common & RIGHT_CLIP)
		ptr->rect.ec = rect->ec;
	}
	else
	{
	    /* if they do not intersect, remove the Region */

	    Remove ((struct Node *)&ptr->node);

	    FreeMem (ptr, sizeof(struct MyRegion));
	}
    }

    if (!GetHead(region))
	status = IS_EMPTY;
    else
	status = NOT_EMPTY;

    return (status);
} /* and_rect_region */


/* make a copy of a region */

static struct RefreshRegion * copy_region (struct RefreshRegion * region)
{
    struct MyRegion * ptr;
    struct RefreshRegion * copy;

    if (copy = new_region ())
    {
	for (ptr=GetHead(&region->list); ptr; ptr=GetSucc(&ptr->node))
	{
	    if (!(add_rect (copy, &ptr->rect)) )
	    {
		dispose_region (copy);

		return (NULL);
	    }
	}
    }

    return (copy);
} /* copy_region */


/* make erase all common areas between two regions */

static void clip_region (struct RefreshRegion * region, struct RefreshRegion * clip)
{
    struct MyRegion * ptr;

    for (ptr=GetHead(&clip->list); ptr; ptr=GetSucc(&ptr->node))
    {
	clear_rect_region (region, &ptr->rect);
    }
} /* clip_region */


/* create a new Region and add it to the RefreshRegion. This routine
   must not be called with a rect that intersects with any of the
   Regions that are already in the RefreshRegion ! */

static BOOL add_rect (struct RefreshRegion * region, struct rect * rect)
{
    struct MyRegion * new;

    if (new = AllocMem (sizeof(struct MyRegion), 0))
    {
	movmem (rect, &new->rect, sizeof (struct rect));

	/* AddHead because we might be editing the list right now :-) */
	AddHead ((struct List *)&region->list, (struct Node *)&new->node);
    }

    return ((BOOL)(new != NULL));
} /* add_rect */


/* adjust regions that none intersects with the rect anymore */

static int clear_rect_region (struct RefreshRegion * region, struct rect * rect)
{
    struct MyRegion * ptr, * next;
    UWORD status;

    for (ptr=GetHead(&region->list); ptr; ptr=next)
    {
	UWORD common;
	struct rect new_rect;

	next = GetSucc (&ptr->node);

	/* do something only if we have an intersection */

	if ((common = do_clip (ptr, rect)) != NO_INTERSECT)
	{
	    /* if both intersect, we make sure we don't have any areas left
	       that are in two Regions */

	    if (common & TOP_CLIP)
	    {
		/* if we have a TOP_CLIP, there must be some new Region
		   above. */

		new_rect.sl = ptr->rect.sl;
		new_rect.el = rect->sl - 1;    /* this is the intersection */
		new_rect.sc = ptr->rect.sc;
		new_rect.ec = ptr->rect.ec;

		/* is this a valid rect anyway ?? */
		if (new_rect.sl <= new_rect.el)
		{
		    if (!add_rect (region, &new_rect))
			return (MEMORY_ERROR);
		    else
			ptr->rect.sl = rect->sl;   /* new topline */
		} /* valid rect ? */
	    } /* type of intersect */

	    if (common & BOTTOM_CLIP)
	    {
		/* if we have a BOTTOM_CLIP, there must be some new Region
		   below. */

		new_rect.sl = rect->el + 1;    /* clipping */
		new_rect.el = ptr->rect.el;
		new_rect.sc = ptr->rect.sc;
		new_rect.ec = ptr->rect.ec;

		/* is this a valid rect anyway ?? */
		if (new_rect.sl <= new_rect.el)
		{
		    if (!add_rect (region, &new_rect))
			return (MEMORY_ERROR);
		    else
			ptr->rect.el = rect->el;
		} /* valid rect ? */
	    } /* type of intersect */

	    if (common & LEFT_CLIP)
	    {
		/* if we have a LEFT_CLIP, there must be some new Region
		   below. */

		new_rect.sl = ptr->rect.sl;
		new_rect.el = ptr->rect.el;
		new_rect.sc = ptr->rect.sc;
		new_rect.ec = rect->sc - 1;    /* clipping */

		/* is this a valid rect anyway ?? */
		if (new_rect.sc <= new_rect.ec)
		{
		    if (!add_rect (region, &new_rect))
			return (MEMORY_ERROR);

		    /* we do not need any ELSE here since RIGHT_CLIP
		       doesn't care about the left border. */

		} /* valid rect ? */
	    } /* type of intersect */

	    if (common & RIGHT_CLIP)
	    {
		/* if we have a RIGHT_CLIP, there must be some new Region
		   below. */

		new_rect.sl = ptr->rect.sl;
		new_rect.el = ptr->rect.el;
		new_rect.sc = rect->ec + 1;    /* clipping */
		new_rect.ec = ptr->rect.ec;

		/* is this a valid rect anyway ?? */
		if (new_rect.sc <= new_rect.ec)
		{
		    if (!add_rect (region, &new_rect))
			return (MEMORY_ERROR);
		} /* valid rect ? */
	    } /* type of intersect */

	    /* remove old area */
	    Remove ((struct Node *)&ptr->node);

	    FreeMem (ptr, sizeof (struct MyRegion));
	} /* intersection */
    }

    if (!GetHead(region))
	status = IS_EMPTY;
    else
	status = NOT_EMPTY;

    return (status);
} /* clear_rect_region */


static int AddDrawEvent (struct RefreshRegion * region, Line sl, Column sc,
	    Line el, Column ec)
{
    struct rect new_rect;

    if (el < sl || ec < sc) /* don't add invalid blocks */
	return (TRUE);

    new_rect.sl = sl;
    new_rect.el = el;
    new_rect.sc = sc;
    new_rect.ec = ec;

    return (add_rect (region, &new_rect));
} /* AddDrawEvent */

static int add_block_draw_events( struct RefreshRegion *region )
  {
    Line sl, el;

    sl = el = ActualBlock.start_line;
    AddDrawEvent (region, sl, block_leftcolumn ( sl ), el, block_rightcolumn( el ) );

    sl = ActualBlock.start_line + 1;
    el = ActualBlock.end_line	- 1;
    AddDrawEvent (region, sl, block_leftcolumn ( sl ), el, block_rightcolumn( el ) );

    sl = el = ActualBlock.end_line;
    AddDrawEvent (region, sl, block_leftcolumn ( sl ), el, block_rightcolumn( el ) );

    return TRUE;  /* Really should check these values some day... */
  }

/*****************************************************************************

    NAME
	redraw_block

    PARAMETER
	BOOL   force;
	Line   start_line;
	Column start_column;
	Line   end_line;
	Column end_column;

    RESULT

    RETURN
	void

    DESCRIPTION
	Draws part of the text (force = TRUE) or adjust the block
	according to the new coordinates (force = FALSE).

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	03. Jan 1978	ada created

******************************************************************************/

Prototype void redraw_block (BOOL force, Line start_line, Column start_column, Line end_line, Column end_column);

void redraw_block (BOOL force, Line start_line, Column start_column, Line end_line, Column end_column)
{
    RP * rp;
    struct RefreshRegion * old,     /* the old block that has to be erased */
			 * new,     /* the new block that has to be drawn */
			 * update,  /* region where we have to redraw the
				       text */
			 * copy;    /* and a copy of old for removing common
				       areas. */
    struct rect new_rect;

    // ActualBlock.flags = 0; /* Should fail; ActualBlock should be const */

    D(bug("----> redraw_block\nforce:%ld    start:%3ld/%3ld  end:%3ld/%3ld\n",
	    force,
	    start_line+1, start_column+1,
	    end_line+1, end_column+1 ));

    if (GETF_COMLINEMODE(Ep))
    {
	text_redraw_cmdline ();
	return;
    }

    old    = new_region ();
    new    = new_region ();
    update = new_region ();

    if (!old || !new || !update)    /* out of memory */
	return;

    if ( !block_ok () || ActualBlock.ep != Ep) /* no block to worry about */
    {
update_only:	     /* jump here if we are sure there is no block to draw */

	D(bug("Update only\n"));

	if (end_line < start_line)
	    SWAP(start_line,end_line);

	if (end_column < start_column)
	    SWAP(start_column,end_column);

	AddDrawEvent (update, start_line, start_column,
		end_line, end_column);

	goto draw;	/* I really don't like goto but I also don't
			   want to indent all those mush below :-) */
    }

    else /* deal with the block stuff */
    /* find drawing positions. After this if, the redraw-struct is filled with
       all information to draw the new situation */
    if (force)
    {
	short part;
	Line sl, el;
	/* here, we must make sure, that start <= end ! */
	if (start_line > end_line)
	{
	    SWAP (start_line,   end_line   );
	    SWAP (start_column, end_column );
	} else if (start_line  == end_line &&
		   start_column > end_column )
	    SWAP (start_column, end_column);

	/* clip end-line */
	if (end_line >= Ep->lines)
	    end_line = Ep->lines-1;

	/* clip all areas against the draw-area */
	new_rect.sl = start_line;
	new_rect.el = end_line;
	new_rect.sc = start_column;
	new_rect.ec = end_column;

		/* The situation is like this :

		  +-----------------------+
		  |	     (A)          |   (A)  The area above the block
		  |	     +---------+  +   (B)  The area in the line where
		  |  (B)     |     (C) |  |        the block starts BEFORE the
		  +  +-------+ - - - - +  +	   block
		  |  |	    (D)        |  |   (C)  The area inside the block in
		  +  + - - - - -+------+  +	   the line where the block
		  |  | (E)      |   (F)   |        begins
		  +  +----------+	  |   (D)  the main body
		  |	     (G)          |   (E)  the rest of the block in the
		  +-----------------------+	   last line
					      (F)  the rest of this line
					      (G)  anything below the block

		*/
	/* region (A) above the block */
	AddDrawEvent (update, start_line, start_column,
			ActualBlock.start_line - 1, end_column);

	for ( part=0; part<3; part++ )
	  {
	    switch (part )
	      {
		case 0: sl =
			el = ActualBlock.start_line;	   break;
		case 1: sl = ActualBlock.start_line + 1;
			el = ActualBlock.end_line   - 1;   break;
		case 2: sl =
			el = ActualBlock.end_line;	   break;
	      } /* switch(part)

	    /* left of block */
	    AddDrawEvent (update, sl, start_column, el, block_leftcolumn(el)-1 );

	    /* right of block */
	    AddDrawEvent (update, sl, block_rightcolumn(sl) + 1, el, end_column );
	  }
	/* region (G) below the block */
	AddDrawEvent (update, ActualBlock.end_line + 1, start_column,
			end_line, end_column);

	/* The block itself (C,D,E) */
	add_block_draw_events( new );

	and_rect_region (update, &new_rect);
	and_rect_region (new, &new_rect);

	/* D(bug("Update :\n")); print_region (update);
	   D(bug("new :\n")); print_region (new); */
    } else /* force == FALSE */
    {
	/* unhighlight the old block */
	add_block_draw_events( old );

	/* update the block coordinates */
	if ( start_line != -1 )
	    set_block_start( start_line, start_column );

	if ( end_line != -1 )
	    set_block_end( end_line, end_column );

	/* highlight the new block */
	add_block_draw_events( new );

	copy = copy_region (old);       /* make a copy of the old block */

	clip_region (old, new);         /* clear all common areas in old */
	clip_region (new, copy);        /* clear all common areas in new */

	dispose_region (copy);          /* free copy */

    } /* if (force) */

draw:	    /* jump here is all DrawEvents are correctly initialized */

    /* Are we allowed to draw ? */
    if (!Nsu && !GETF_ICONMODE(Ep))
    {
	/* init rastport */
	rp = Ep->win->RPort;

	/* clip the update-region */
	new_rect.sl = Ep->topline;
	new_rect.el = new_rect.sl + Lines - 1;
	new_rect.sc = Ep->topcolumn;
	new_rect.ec = new_rect.sc + Columns - 1;

	/* make sure we don't get below the last line */
	if (new_rect.el >= Ep->lines)
	    new_rect.el = Ep->lines - 1;

	/* clip away everything that exceeds the displayable area */
	and_rect_region (update, &new_rect);
	and_rect_region (old, &new_rect);
	and_rect_region (new, &new_rect);

	refresh_region (rp, update, 2);
	refresh_region (rp, old, 0);
	refresh_region (rp, new, 1);

	dispose_region (update);
	dispose_region (old);
	dispose_region (new);
    }
} /* redraw_block */


static void refresh_region (RP * rp, struct RefreshRegion * region, int mode)
{
    struct MyRegion * ptr;
    Line start_line, end_line;
    Column start_column, end_column;
    D(int t=0);

    for (ptr=GetHead(&region->list); ptr; ptr=GetSucc(&ptr->node))
    {
	start_line   = ptr->rect.sl;
	end_line     = ptr->rect.el;
	start_column = ptr->rect.sc;
	end_column   = ptr->rect.ec + 1;

	D(bug("Draw %ld: %ld (%2ld,%2ld) - (%2ld/%2ld)\n", t ++,
	    mode, start_line+1, start_column+1, end_line+1, end_column+1));

	if (start_line > end_line ||
	    start_column >= end_column)
	{
	    D(bug("ERROR !!!!\n"));
	    continue;
	}

	if (mode != 2)
	{
	    if (mode == 1)
		SetAPen (rp, BLOCK_BPEN(Ep));
	    else
		SetAPen (rp, TEXT_BPEN(Ep));

	    SetWrMsk (rp, BLOCK_MASK(Ep));
	    SetDrMd (rp, JAM2);

	    RectFill (rp, COL(start_column - Ep->topcolumn),
			ROW(start_line - Ep->topline),
			COL(end_column - Ep->topcolumn)-1,
			ROW(end_line + 1 - Ep->topline)-1 );

	    SetWrMsk (rp, ALL_MASK);
	}

	for ( ; start_line <= end_line; start_line ++)
	    redraw_textlineseg (start_line, start_column, end_column);
    }
} /* refresh_region */


/******************************************************************************
*****  ENDE redraw.c
******************************************************************************/
