#include "dopuslib.h"

/* NOTE !! This file is rewritten from the graphics.a asm file */

#define DB(x) ((struct DOpusBase *)x)

/*****************************************************************************

    NAME */

	AROS_LH7(void, Do3DBox,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp, A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, w, D2),
	AROS_LHA(int, h, D3),
	AROS_LHA(int, tp, D4),
	AROS_LHA(int, bp, D5),

/*  LOCATION */
	struct Library *, DOpusBase, 6, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/

{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	int new_x, new_y, bot_y, right_x;
	
	SetAPen(rp, tp);
	
	new_x = x - 2;
	new_y = y - 1;
	Move(rp, new_x, new_y);
	
	Draw(rp, x + w, new_y);
	
	Move(rp, new_x, new_y);
	
	Draw(rp, new_x, y + h);
	
	new_x = x - 1; /* AROSSG */
	Move(rp, new_x, new_y);
	
	bot_y = y + h;
	Draw(rp, new_x, bot_y - 1);
	
	SetAPen(rp, bp);
	
	Move(rp, new_x, bot_y);
	
	right_x = x + w;
	
	Draw(rp, right_x + 1, bot_y);
	
	Draw(rp, right_x + 1, y - 1);
	
	Move(rp, right_x, bot_y);
	
	Draw(rp, right_x, y);
	
	
    
	AROS_LIBFUNC_EXIT
    
}


/*****************************************************************************

    NAME */

	AROS_LH7(void, Do3DStringBox,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp, A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, w, D2),
	AROS_LHA(int, h, D3),
	AROS_LHA(int, tp, D4),
	AROS_LHA(int, bp, D5),

/*  LOCATION */
	struct Library *, DOpusBase, 7, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/

{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	int left_x, top_y, right_x, bot_y;
	Do3DBox(rp, x - 2, y - 1, w + 4, h + 2, tp, bp);
	
	SetAPen(rp, bp);
	
	left_x = x - 2;
	top_y  = y - 1;
	
	Move(rp, left_x, top_y);
	Draw(rp, x + w + 1, top_y);
	
	Move(rp, left_x, top_y);
	Draw(rp, left_x, y + h + 1);
	
	left_x = x - 1;
	Move(rp, left_x, top_y);
	
	bot_y = y + h;
	
	Draw(rp, left_x, bot_y - 1);
	
	SetAPen(rp, tp);
	
	Move(rp, left_x, bot_y);
	
	right_x = x + w + 1;
	
	Draw(rp, right_x, bot_y);
	Draw(rp, right_x, y - 1);
	
	right_x --;
	Move(rp, right_x, bot_y);
	Draw(rp, right_x, y);
	

    
	AROS_LIBFUNC_EXIT
    
}


/*****************************************************************************

    NAME */

	AROS_LH7(void, Do3DCycleBox,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp, A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, w, D2),
	AROS_LHA(int, h, D3),
	AROS_LHA(int, tp, D4),
	AROS_LHA(int, bp, D5),

/*  LOCATION */
	struct Library *, DOpusBase, 8, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/

{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	ULONG old_drmd;
	
	int new_x;
	int new_y = y;
	int new_h;

	int tmpl_x, tmpl_y;
kprintf("Do3DCycleBox\n");

kprintf("CYCLETOP: %p, CYCLEBOT: %p\n"
	, DB(DOpusBase)->pdb_cycletop
	, DB(DOpusBase)->pdb_cyclebot
);
	
	
	Do3DBox(rp, x, y, w, h, tp, bp);
	
	SetAPen(rp, bp);
	
	old_drmd = GetDrMd(rp);
	
	SetDrMd(rp, JAM1);
	
	if (DB(DOpusBase)->pdb_cycletop) {
		
		tmpl_x = x + 4;
		tmpl_y = y + 1;
		
		new_y = h;
		new_y -= 12;
		new_y += tmpl_y;
		new_y += 7;
		
		
		new_h = h - 12 + h + 7;
		BltTemplate( (PLANEPTR)DB(DOpusBase)->pdb_cycletop
			, 0, 2, rp, tmpl_x, tmpl_y, 11, 6);
			
			
	}
	
	if (DB(DOpusBase)->pdb_cyclebot) {
		BltTemplate((PLANEPTR)DB(DOpusBase)->pdb_cyclebot
			, 0, 2, rp, tmpl_x, new_y, 11, 2);
	}
	
	
	new_x = x + 4;
	new_y = y + 7;
	
	Move(rp, new_x, new_y);
	
	
	new_h = h - 12;
	
	Draw(rp, new_x, new_y + new_h);
	
	new_x = x + 5;
	Move(rp, new_x, new_y);
	
	Draw(rp, new_x, new_y + new_h);
	
	new_x = x + 18;
	new_y = y + 1;
	
	Move(rp, new_x, new_y);
	
	new_h = h - 2;
	Draw(rp, new_x, y + new_h);
	
	SetAPen(rp, tp);
	
	
	new_x = x + 19;
	
	Move(rp, new_x, new_y);
	
	Draw(rp, new_x, y + new_h);
	
	SetDrMd(rp, old_drmd);
	

	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH4(void, DrawCheckMark,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp, A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, checked, D2),

/*  LOCATION */
	struct Library *, DOpusBase, 44, DOpus)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	ULONG old_drmd;
	UBYTE old_apen;

	old_drmd = GetDrMd(rp);
	
	SetDrMd(rp, JAM1);
	
	if (!checked) {
		old_apen = GetAPen(rp);
		
	
		SetAPen(rp, GetBPen(rp));
	} else SetAPen(rp, GetAPen(rp));
	
	if (DB(DOpusBase)->pdb_check) {

		BltTemplate( (PLANEPTR)DB(DOpusBase)->pdb_check
			,0, 2, rp, x, y, 13, 7);
	}
	
	if (!checked) {
		SetAPen(rp, old_apen);
	}
	
	SetDrMd(rp, old_drmd);
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH2(void, HiliteGad,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, gad, A0),
	AROS_LHA(struct RastPort *, rp, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 67, DOpus)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	ULONG old_drmd;
	
	old_drmd = GetDrMd(rp);
	SetDrMd(rp, COMPLEMENT);
	
	RectFill(rp
		, gad->LeftEdge, gad->TopEdge
		, gad->LeftEdge + gad->Width  - 1
		, gad->TopEdge  + gad->Height - 1
	);
	
	SetDrMd(rp, old_drmd);
	
	AROS_LIBFUNC_EXIT
}
